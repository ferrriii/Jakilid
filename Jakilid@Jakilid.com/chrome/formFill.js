//using Module Pattern
var formFill = function(){

	//jquery unique selector: http://stackoverflow.com/questions/2068272/getting-a-jquery-selector-for-an-element
	jQuery.fn.getPath = function () {
		if (this.length != 1) throw 'Requires one element.';

		var path, node = this;
		while (node.length) {
			var realNode = node[0], name = realNode.localName;
			if (!name) break;
			name = name.toLowerCase();

			var parent = node.parent();

			var siblings = parent.children(name);
			if (siblings.length > 1) {
				name += ':eq(' + siblings.index(realNode) + ')';
			}

			path = name + (path ? '>' + path : '');
			node = parent;
		}

		return path;
	};

	function findNearestNumber(theArray, goal)
	{
		var closest = null;
		var closestIndex = -1;

		$.each(theArray, function(index){
		  if (closest == null || Math.abs(this - goal) < Math.abs(closest - goal)) {
			closest = this;
			closestIndex = index
		  }
		});

		return closestIndex;
	}

	/*!
	 * fills accObj infor-pair values into apropriate preferred form input fields
	 */
	function fillAcc(accObj)	//public
	{
		//remove infopairs of type of URL or Tags (they won't get filled anywhere)
		for (var t=0; t< accObj.infoPairs.length; t++)
		{
			var info = accObj.infoPairs[t];
			if (info.type == 4 || info.type == 5)	//4=URL, 5=Tags
				accObj.infoPairs.splice(t--,1);
		}

		var forms = pageForms();		//extract page form elements
		//find the best matched form to accObj: choose form which has minimum difference in form inputs and account infoPairs count
		var formInputCountArray = [];
		for (var i = 0; i < forms.length; i++)
			formInputCountArray.push(forms[i].inputs.length);
		var preferedForm = forms[ findNearestNumber(formInputCountArray, accObj.infoPairs.length)  ];
		
		var filledForm = JakilidWrapper.fillForm(preferedForm, accObj);

		if (filledForm.filledInputs <=0)
			return;

		for (var i = 0; i < filledForm.inputs.length; i++)
		{
			var formInput = filledForm.inputs[i];
			$(formInput.selector, (formInput.context || content.document)).val(formInput.value);
		}
		$(preferedForm.selector, (preferedForm.context || content.document)).attr('data-autofilled', 1);	//mark form as auto filled
	}

	function accountFromForm(formObj)	//public
	{
		//this function assume formObj has been cleaned already. it means there should be no submit,button,select,etc input in the form
		var infoPairsArray = [],
			userName,
			location = gBrowser.contentWindow.location;//.href;

		var inputCnt = 0;
		for (var i = 0; i < formObj.inputs.length; i++)
		{
			var input = formObj.inputs[i];

			if (typeof input.value === 'undefined' || input.value == '')	//ignore empty inputs?
				continue;
			if (/captcha|security.?code|pattern|image/i.test(input.name))	//TODO: check against locale
				continue;

			inputCnt++;
			//find key-part in input placeHolder,name,id, class? as input name
			var inpName = $("label[for='" + input.id + "']", content.document).text();			//find label associated to this input. NOTE: preceding element of current input could be used as label but would be less accurate!
			if (inpName)
				inpName = cleanInputLabel(inpName);							//trim, remove :, etc. from input label
			//if (typeof i18n.lang.suggestInputName === 'function')
			//	inpName = i18n.lang.suggestInputName(input, i, formObj.inputs);
			if (!inpName)
				inpName = suggestInputTitle(input);							//try finding input name its attributes like placeholder,name, id, title, etc.
			if (!inpName)													//fallback to original input name
				inpName = cleanInputName(input.name);						//remove prefix,suffix like txt,_, inp,ctl, etc.
			//translate inpName
			inpName = tryTranslate(inpName);
			var inp = newInfo(inpName, input.value, infoTypeId(input.type));
			inp.order = (inputCnt);
			inp.formInput = input.name;	//save original form input name for saving in database

			if (inputCnt==1 && inp.type != KEYTYPE_PASSWORD)	//if first input is not password assume it as username (would be used for account title)
				userName = inp.value;

			infoPairsArray.push(inp);
		}
		//remove confirmation inputs (password, email)
		for(var i = 0; i < infoPairsArray.length; i++)
		{
			if (infoPairsArray[i].type != KEYTYPE_PASSWORD)
				continue;

			var confirmInfoIndex =  duplicateInfoIndex(infoPairsArray[i], i, infoPairsArray);
			if (confirmInfoIndex>0)
				infoPairsArray.splice(confirmInfoIndex, 1);	//remove duplicated( confirmed) info
		}
		if (infoPairsArray.length == 0)
			return;
		//add url as an info
		var inp = newInfo(tr("Login URL",'account'), location.href, KEYTYPE_URL);
		inp.order = ++inputCnt;
		infoPairsArray.push(inp);

		var acc = {id : -1,
					title: 		suggestAccTitle(userName, location.hostname),
					infoPairs:	infoPairsArray
					};

		return acc;
	}
	
	function tryTranslate(label)
	{
		var orgLabel = label.toLowerCase();
		var trLabel = tr(orgLabel, 'form');
		if (trLabel == orgLabel)		//try translating label removing spaces
		{
			orgLabel = orgLabel.replace(/\s/gm, '');
			trLabel = tr(orgLabel, 'form');
			if (trLabel == orgLabel)
				trLabel = label;	//fallback to original label
		}
		return trLabel;
	}

	function duplicateInfoIndex(info, index, infoArray)
	{
		if (!info.value)
			return 0;
		for(var i = index+1; i < infoArray.length; i++)
		{
			if (info.value == infoArray[i].value && info.type == infoArray[i].type)
				return i;
		}
		return 0;
	}

	function cleanInputLabel(str)
	{
		var patt = /^(\s+|:+|-|_)|(\*.*|\..*|,.*|\s+|:.*|\(.*)$/gm; // translates "  _user id (email, phone.) : " to "user id" 
		while (patt.test(str))
			str = str.replace(patt,'');

		return str;
	}

	function cleanInputName(str)
	{
		str = str.replace(/^.+\$/, '');	//remove .net prefix
		var patt = /^(txt|inp|.?_|ctl|lbl)/i;
		while (patt.test(str))
			str = str.replace(patt, '');
		str = str.replace(/(_|_.)$/, '');

		return str;
	}

	function suggestInputTitle(input)
	{
		if (input.type && input.type.toLowerCase() == 'password' && (!input.name || tr(input.name.toLowerCase(), 'form') == input.name))
			return 'password';	//if input is password and there is no direct translation for input name then name it as password

		if (input.placeHolder)
		{
			var s = input.placeHolder.toLowerCase();
			var n = cleanName(s);
			if (n)
				return n;
		}

		if (input.name)
		{
			var s = input.name.toLowerCase();
			var n = cleanName(s);
			if (n)
				return n;
		}

		if (input.id)
		{
			var s = input.id.toLowerCase();
			var n = cleanName(s);
			if (n)
				return n;
		}

		return undefined;
	}

	function cleanName(s)
	{
		//TODO: there should be a better way for internationalization!
		if (s.indexOf('password') >=0 || s.toLocaleString().indexOf(tr('password','form')) >=0 )
			return 'password';
		if (s.indexOf('passwd') >=0 || s.toLocaleString().indexOf(tr('passwd','form')) >=0 )
			return 'password';
		if (s.indexOf('tell') >=0 || s.toLocaleString().indexOf(tr('tell','form')) >=0 )
			return 'tell';
		if (s.indexOf('username') >=0 || s.toLocaleString().indexOf(tr('username','form')) >=0 )
			return 'username';
		if (s.indexOf('pass') >=0 || s.toLocaleString().indexOf(tr('pass','form')) >=0 )
			return 'password';
		if (s.indexOf('usrname') >=0 || s.toLocaleString().indexOf(tr('usrname','form')) >=0 )
			return 'username';
		if (s.indexOf('login') >=0 || s.toLocaleString().indexOf(tr('login','form')) >=0 )
			return 'login';
		if (s.indexOf('mail') >=0 || s.toLocaleString().indexOf(tr('mail','form')) >=0 )
			return 'email';
		if (s.indexOf('lastname') >=0 || s.toLocaleString().indexOf(tr('lastname','form')) >=0 )
			return 'lastname';
		if (s.indexOf('firstname') >=0 || s.toLocaleString().indexOf(tr('firstname','form')) >=0 )
			return 'firstname';
		if (s.indexOf('name') >=0 || s.toLocaleString().indexOf(tr('name','form')) >=0 )
			return 'name';
		if (s.indexOf('mobile') >=0 || s.toLocaleString().indexOf(tr('mobile','form')) >=0 )
			return 'mobile';
		if (s.indexOf('number') >=0 && (s.indexOf('cell') >=0 || s.indexOf('phone') >=0))
			return 'phone number';
		if (s.indexOf('pwd') >=0 || s.toLocaleString().indexOf(tr('pwd','form')) >=0 )
			return 'pwd';
		if (s.indexOf('address') >=0 )
			return 'address';
		if (s.indexOf('phone') >=0 )
			return 'phone';
		if (s.indexOf('user') >=0 || s.toLocaleString().indexOf(tr('user','form')) >=0 )
			return 'user';
		if (s.indexOf('usr') >=0 )
			return 'username';

		return undefined;
	}

	function pageForms()	//public
	{
		var forms = [];
		forms = getForms(content.document);
		
		$('iframe',content.document).each( function()
		{
			forms = forms.concat(getForms( $(this).contents() ));
		});
		
		return forms;
	}

	function getForms(context)
	{
		var forms = {};
		$('form:visible input:visible',context).each(function( )
		{
			var $inp = $(this);
			var inp = inputFromElement($inp, context);
			if (!inp)
				return;

			var form = $inp.closest("form");
			var formSelector = form.getPath();
			if (typeof forms[formSelector] === 'undefined')
			{
				forms[formSelector] =
				{
					id : form.attr('id'),
					referer : gBrowser.contentWindow.location.href, //window.location.pathname,
					action : form.action,
					selector: formSelector,
					'context': context && context != content.document ? context : undefined,	//if context if not in iframe (current document) do not store it
					inputs: []
				};
			}
			forms[formSelector].inputs.push(inp);
		});


		var formsArray = [];
		for (var key in forms)
		{
			//remove search forms
			var frm = forms[key];
			setFormRanking(frm);
			if (frm.searchRank < frm.loginRank && frm.searchRank < 20 && frm.loginRank>=10)	//for search forms searchRank is usually greater than 20 and for login forms loginRank is at least 10
				formsArray.push(frm);
		}

		return formsArray;
	}

	function formFromElement(f)	//public
	{
		var $form = $(f, content.document);
		var fObj =
		{
			id : $form.attr('id'),
			referer : gBrowser.contentWindow.location.href, //window.location.pathname,
			action : $form.action,
			selector: $form.getPath(),
			'context': undefined,	//undefinedi is default
			autoFilled: $form.attr('data-autofilled'),
			inputs: []
		};

		$(f,content.document).find('input:visible').each(function( )
		{
			var $inp = $(this);
			var inp = inputFromElement($inp);
			if (inp)
				fObj.inputs.push(inp);
		});

		setFormRanking(fObj);
		return fObj;
	}

	function inputFromElement($inp, context)
	{
		var infputType = $inp.attr('type') || '';
		infputType = infputType.toLowerCase();
		if (infputType == 'radio' || infputType == 'submit' || infputType == 'checkbox' || infputType == 'search' || infputType == 'button' || infputType == 'file' || infputType == 'hidden' || infputType == 'image' || infputType == 'reset')
			return undefined;	//ignore non text fields
		if ($inp.attr('readonly') || $inp.attr('disabled'))
			return undefined; //ignore nonEditable inputs

		return {
			//TODO: add label
			name: $inp.attr('name'),
			value: $inp.val(),
			title: $inp.attr('title'),
			type: $inp.attr('type'),
			id: $inp.attr('id'),
			placeHolder:$inp.attr('placeholder'),
			selector: $inp.getPath(),
			'context': context && context != content.document ? context : undefined
		};
	}

	function setFormRanking(frm)
	{
		frm.searchRank = 0;
		frm.loginRank=0;
		if (frm.id)
		{
			frm.searchRank += frm.id.indexOf('search') >= 0 || frm.id.indexOf('query') >= 0 || frm.id.indexOf('find') >= 0  ? 7 : 0;
			frm.loginRank += frm.id.indexOf('login') >= 0 || frm.id.indexOf('signin') >= 0 || frm.id.indexOf('logon') >= 0  ? 7 : 0;
		}
		if (frm.referer)
		{
			frm.searchRank += frm.referer.indexOf('search') >= 0 || frm.referer.indexOf('query') >= 0 || frm.referer.indexOf('find') >= 0  ? 2 : 0;
			frm.loginRank += frm.referer.indexOf('login') >= 0 || frm.referer.indexOf('signin') >= 0 || frm.referer.indexOf('logon') >= 0  ? 2 : 0;
		}
		if (frm.action)
		{
			frm.searchRank += frm.action.indexOf('search') >= 0 || frm.action.indexOf('query') >= 0 || frm.action.indexOf('find') >= 0  ? 10 : 0;
			frm.loginRank += frm.action.indexOf('login') >= 0 || frm.action.indexOf('signin') >= 0 || frm.action.indexOf('logon') >= 0  ? 10 : 0;
		}
		frm.searchRank += frm.inputs.length == 1 ? 5 : 0;

		for (var i = 0; i < frm.inputs.length; i++)
		{
			var inp = frm.inputs[i];
			if (inp.name)
			{
				frm.searchRank += inp.name.indexOf('search') >= 0 ||  inp.name.indexOf('query') >= 0 ||  inp.name.indexOf('find') >= 0 ? 9 : 0;
				frm.searchRank += inp.name == 'q'  ? 5 : 0;
				frm.loginRank += inp.name.indexOf('user') >= 0 ||  inp.name.indexOf('pass') >= 0 ||  inp.name.indexOf('login') >= 0 ? 9 : 0;
			}
			if (inp.id)
			{
				frm.searchRank += inp.id.indexOf('search') >= 0 ||  inp.id.indexOf('query') >= 0 ||  inp.id.indexOf('find') >= 0 ? 8 : 0;
				frm.loginRank += inp.id.indexOf('user') >= 0 ||  inp.id.indexOf('pass') >= 0 ||  inp.id.indexOf('login') >= 0 ? 8 : 0;
			}
			if (inp.placeHolder)
			{
				frm.searchRank += inp.placeHolder.indexOf('search') >= 0 ||  inp.placeHolder.indexOf('query') >= 0 ||  inp.placeHolder.indexOf('find') >= 0 ? 10 : 0;
				frm.loginRank += inp.placeHolder.indexOf('user') >= 0 ||  inp.placeHolder.indexOf('pass') >= 0 ||  inp.placeHolder.indexOf('login') >= 0 ? 10 : 0;
			}
			if (inp.type) frm.loginRank += inp.type.toLowerCase() == 'password' ? 10 : 0;
		}
	}

	//return public methods
	return {
		formFromElement:formFromElement,
		pageForms:pageForms,
		accountFromForm: accountFromForm,
		fillAcc: fillAcc
	};

}();
