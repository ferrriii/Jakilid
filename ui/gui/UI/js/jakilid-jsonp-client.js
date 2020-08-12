function JakilidJsonpClient(sid)
{
//	if (!('WebSocket' in window))
//		return false;	//WARNING: websocket is not supported by browser


	this.onLock = null;
	this.onUnlock = null;
	this.onReady = null;
	this.onError = null;	//occurs on any error (recoverable or not!)
	this.prepared = 0;	//whether initial connection was stablished
	this.ready = 0;
	var self = this;
	this.unlocked = false;
	
	var sid = sid || Math.random().toString().substr(2);	
	this.sid = function()
	{
		return sid;
	}
	
	this.isReady = function()
	{
		return this.ready;
	};
		
	function send(cmd, msg, callBack)
	{
		return $.ajax({
			url: "http://127.0.0.1:55886/" + sid + '/',
			type: "get",
			dataType: 'jsonp',
			data: {'cmd': cmd, data: JSON.stringify(msg) }
		})
		.done(callBack);
	}

	/* ------------- Jakilid functions -------------- */

	this.isUnlocked = function(callBack)
	{
		return send( 'isUnlocked', undefined, function(nret)
		{
			if (!self.ready)
			{
				self.ready = 1; 
				typeof self.onReady === 'function' && self.onReady();
			}
			
			if (self.unlocked != nret)
			{
				if (nret && typeof self.onUnlock === 'function')
					self.onUnlock();
				else if (!nret && typeof self.onLock === 'function')
					self.onLock();

				self.unlocked = nret;
			}
			callBack(nret); 
		} )
		.fail(function()
		{
			if (self.ready)
			{
				self.ready = 0;
				typeof self.onError === 'function' && self.onError();
			}
		});
	}
	
	//check server availability
	this.isUnlocked( $.noop );
	setInterval( function(){ self.isUnlocked( $.noop ); }, 10000 );
	
	
	this.find = function(what, tag, index, offset, callBack)
	{
		var findJsonRequest = 
		{
			'what': what,
			'label': tag,
			'index': index,
			'offset': offset
		};
		return send( 'find', findJsonRequest, callBack );
	}

	this.labels = function(callBack)
	{
		return send( 'labels', undefined, callBack );
	}

	this.upgradeDb = function(callBack)
	{
		return send( 'upgradeDb', undefined, callBack );
	}	
	
	this.findByUrl = function(url, index, offset, callBack)
	{
		var findJsonRequest = 
		{
			'url': url,
			'index': index,
			'offset': offset
		};
		return send( 'findByUrl', findJsonRequest, callBack );
	}	
	
	this.authenticate = function(profileName, pass, callBack)
	{
		var jsonCmd = 
		{
			'profile': profileName,
			'pass': pass
		}
		var auth = send( 'authenticate', jsonCmd, callBack );
		this.isUnlocked( $.noop );
		return auth;
	}	

	this.about = function(callBack)
	{
		return send( 'about', undefined, callBack );
	}
	
	this.lock = function()		//NOTE: there's no return value for lock so there would be no callback
	{
		var l = send( 'lock' );
		this.isUnlocked( $.noop );
		return l;
	}
	
	this.ping = function()
	{
		return send( 'ping' );
	}
	
	this.save = function(account, callBack)
	{
		return send( 'save', account, callBack );
	}

	this.load = function(id, callBack)
	{
		return send( 'load', id, callBack );
	}
	
	this.remove = function(id, callBack)
	{
		return send( 'remove', id, callBack );
	}
	
	this.getCurrentProfile = function(callBack)
	{
		return send( 'getCurrentProfile', null, callBack );
	}
	
	this.setSettings = function(timeout, lang, callBack)
	{
		var jsonCmd = 
		{
			'timeout': timeout,
			'lang': lang
		}
		return send( 'setSettings', jsonCmd, callBack );
	}

	this.changePass = function(curPass, newPass, callBack)
	{
		var jsonCmd = 
		{
			'curPass': curPass,
			'newPass': newPass
		}
		return send( 'changePass', jsonCmd, callBack );
	}
	
	this.getAvailableProfiles = function(callBack)
	{
		return send( 'getAvailableProfiles', null, callBack );
	}
	
	this.createProfile = function(name, db, lang, pass, isNew, callBack)
	{
		var jsonCmd = 
		{
			'name': name,
			'db': db,
			'lang': lang,
			'pass': pass,
			'isNew': isNew
		}
		return send( 'createProfile', jsonCmd, callBack );
	}
	
	this.isPassStrong = function(pass, callBack)
	{
		return send( 'isPassStrong', pass, callBack );
	}
	
	this.fillForm = function(formObj, accObj)
	{
		var filledForm = {filledInputs: 0, inputs: []};
		//NOTE: accObj should not have infoPairs of type of URL, Tags because all inforPairs will take into account for filling
		//BUG: removing accObj inforPairs will corruct it for further use
		for (var x = 0; x < accObj.infoPairs.length; x++)
			accObj.infoPairs[x].initOrder = x;		//save initial account order for further using for ranking
		
		for (var i = 0; i < formObj.inputs.length; i++)
		{
			var input = formObj.inputs[i];
			if (/search|query|find/igm.test(input.name))	//ignore search inputs in login forms (wrong page design, it's a rare case!)
				continue;
			
			if (accObj.infoPairs.length <= 0)
				break;
			
			//calculate matching rank for each account infoPair
			//TODO: instead of finding first best match for each input, calculate matchrank every possible input-info and choose the best match
			for (var x = 0; x < accObj.infoPairs.length; x++)
			{
				var info = accObj.infoPairs[x];
				
				info.rank = calcMatchRank(input, info);
				if (i==info.initOrder)
					info.rank += 10;	//add extra 10 points if info and input share same index
				//alert(input.name + ', ' + info.key + ' : ' + info.rank);
			}

			//find best info match for input
			var matchedInfoIndex = findBestMatch(accObj.infoPairs);
			if (matchedInfoIndex > -1)
			{
				//alert('matched:' + accObj.infoPairs[matchedInfoIndex].key);
				filledForm.filledInputs++;
				input.value = accObj.infoPairs[matchedInfoIndex].value;
				filledForm.inputs.push(input);
				//remove info from list to avoid filling duplicated value for different inputs
				accObj.infoPairs.splice(matchedInfoIndex, 1);
			}
		}

		return filledForm;
	}
	
	
	
	
	//function used for auto form fill
	
	function calcMatchRank(input, info)
	{
		//TODO: take input.label into accout
		var rank = 6 * letterPairSimilarity(input.placeHolder, info.key);
		rank += 10 * letterPairSimilarity(input.name, info.formInput);
		rank +=5 * letterPairSimilarity(input.name, info.key);
		if (infoTypeName(info.type) != 'text')
			rank +=4 * letterPairSimilarity(input.type, infoTypeName(info.type));
		rank +=2 * letterPairSimilarity(input.id, info.key);
			
		var inputList = infoInputs(info.key.toLowerCase());	//find inputs for similar info
		var p=0;
		for (var i = 0; i < inputList.inputs.length; i++)
		{
			//alert(inputList.inputs[i] + ' ' + info.key + ' ' + letterPairSimilarity(inputList.inputs[i], input.name) + ' ' + inputList.rank);
			p += 4 * letterPairSimilarity(inputList.inputs[i], input.name);
			p += 1 * letterPairSimilarity(inputList.inputs[i], input.id);
			p += 5 * letterPairSimilarity(inputList.inputs[i], input.placeHolder);
		}
		rank += (p * inputList.rank)/100;
		
		//alert('input:' + input.name + "\n" + 			'info:' + info.key + "\n" + 				'info input:' + info.formInput + "\n" +				'p:' + p + "\n" + 	'similarity:' + inputList.rank + "\n" + "rank:" + rank)
		return rank;
	}

	function infoInputs(infoKey)
	{
		if (typeof i18n.lang !== 'object' || typeof i18n.lang.context !== 'object' || typeof i18n.lang.context.formFill !== 'object')
			return {rank:0, inputs:[]};
		
		var formFill = i18n.lang.context.formFill;
		var inputsArray = formFill[infoKey];	//look for inputs associated with exact info
		if (typeof inputsArray === 'object')
			return {rank:100, inputs:inputsArray}; 
		
		//there's not exact match find similars
		
		var similarInfo = Object.keys(formFill);
		var si = [];
		
		for (var i = 0; i < similarInfo.length; i++)
		{
			similarInfo[i].rank
			
			var inf = {'i': similarInfo[i]};
			inf.rank = letterPairSimilarity(inf.i, infoKey);
			
			si.push(inf);
		}
		
		var matchedInfoIndex = findBestMatch(si);
		if (matchedInfoIndex<= -1)
			return {rank:0, inputs:[]};
		var matchedInfo = si[matchedInfoIndex];
		if (matchedInfo.rank < 35)	//ignore row ranked info
			return {rank:0, inputs:[]};
		
		
		return {rank:matchedInfo.rank, inputs:formFill[matchedInfo.i]}; 
	}


	function findBestMatch(infoArray)
	{
		var maxRank = 0;
		var maxRankedIndex = -1;
		for (var x = 0; x < infoArray.length; x++)
		{
			if (infoArray[x].rank > maxRank)
			{
				maxRank = infoArray[x].rank;
				maxRankedIndex = x;
			}
		};
		
		return maxRankedIndex;
	}

	/* reference:http://www.catalysoft.com/articles/StrikeAMatch.html with some changes*/
	function letterPairSimilarity(s1, s2)
	{
		if (typeof s1 === 'undefined' || typeof s2 === 'undefined')
			return 0;
		var s1_simple = s1.toString().toLowerCase().replace(/\s+/,' ');	//TODO: trim
		var s2_simple = s2.toString().toLowerCase().replace(/\s+/,' ');	//TODO: trim

		var s1_pairs = [];
		var s2_pairs = [];

		var s1Words = s1_simple.split(/[ ,_]+/);
		for (var x = 0; x < s1Words.length; x++)
		{
			var word = s1Words[x];
			var s1PairsCount = word.length - 1; //BUG: remove -1 if you want to avoid 100% similarity for (vitamin a, vitamin b)

			for (var i=0; i< s1PairsCount; ++i)
				s1_pairs.push(word.substr(i,2));
		}


		var s2Words = s2_simple.split(/[ ,_]+/);
		for (var x = 0; x < s2Words.length; x++)
		{
			var word = s2Words[x];
			var s2PairsCount = word.length - 1; //BUG: remove -1 if you want to avoid 100% similarity for (vitamin a, vitamin b)

			for (var i=0; i< s2PairsCount; ++i)
				s2_pairs.push(word.substr(i,2));
		}

		var match = 0;

		for (var x = 0; x < s1_pairs.length; x++)
		{
			var pair = s1_pairs[x];
			if (s2_pairs.indexOf(pair) > -1)
				++match;
		}

		return 200 * match / (s1_pairs.length + s2_pairs.length);
	}
	
}
