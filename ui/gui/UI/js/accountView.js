function AccountViewer()
{

	/*************** events *******************/
	this.onOk	=		function(){that.close();};
	this.onEdit =		function(){that.startEdit();};
	this.onDelete =		null;
	this.onSave =		function(){};
	this.onCancel =		function(){that.cancelEdit();};
	this.onNewInfo =	function(){};
	this.onAccountChange = null;
	this.beforeClose = 	null;
	this.onCopy =		null;
	
	/*************** private ****************/
	var m_state =	'closed';
	var m_isChanged = false;
	var globalId = 0;
	var that = this;	//keep reference (allow private functions access public members,functions)
		
	var rainbow = new Rainbow();
	rainbow.setSpectrum('b22222', 'ffde00', '009933');

	/************** getters *******************/
	
	this.isEdit = function() 
	{
		return m_state == 'edit'; 
	}
	
	this.isShow = function() 
	{
		return m_state == 'show'; 
	}
	
	this.isVisible = function() 
	{
		return m_state != 'closed'; //$("#account").dialog( "isOpen" );
	}
	
	this.state = function()
	{
		return m_state;
	}
	
	this.isChanged = function()
	{
		return m_isChanged;
	}
	
	/*************** methods ********************/
	
	this.show = /*bool*/ function(accountObj)
	{
		if (typeof accountObj !== 'object' /*|| this.isVisible()*/)
			return false;

		this.clear();		//remove previous account info and titles (if any)
		m_state = 'show';	//set state to show
		this.cancelEdit();	//remove editables from account title,etc
		fillAccount(accountObj);	//fill current account info, title, etc
		this.setChanged(false);	//set changed to false
		setupFormAsShow();		//setup display form
		
		$("#account").dialog("open"); //open form
		return true;
	}
	
	this.edit = /*bool*/ function(accountObj)
	{
		if (typeof accountObj !== 'object' /*|| this.isVisible()*/)
			return false;
		
		//show account then edit
		if (this.show(accountObj))
			return this.startEdit();
		else
			return false;
			
		// this.clear();			//remove previous account info, title, etc
		// m_state = 'edit';		//set state to edit
		// this.setChanged(false);			//set changed state to false
		// fillAccount(accountObj);	//fill current account info,title,etc
		// this.startEdit();				//make current account info,title,etc editable
		////NOTE: startEdit and fillAccount both call inlinEdit, once is enough (performance boost)
		// $("#account").dialog("open"); 	//open form		
		// return true;		
	}
	
	this.close = /*void*/ function() 
	{
		$('#account').dialog( "close" );
	}
	
	this.addInfo = /*void*/ function(info, isMute /*optional boolean*/) 
	{
		infoUid = getNextUID();//globalId++;
		infoTable = $('#account-info > tbody');

		var keyEl = $('<td></td>', {'class': 'minwidth'}).append( $('<span></span>', {id: 'k-' + infoUid, 'data-type': 'text', 'data-maxwidth': '115', 'data-required':'1', 'class':"infoKey", 'data-title':tr('Enter info title', 'account'), text:info.key}) );
		var valueEl = $('<td></td>', {style: 'position:relative'});
		var controlEl = $('<div></div>', {'class': 'absFright', style: 'position:absolute;right:10px;top:10px;'});
		controlEl.append( $('<span></span>', {id: 'copy-' + infoUid, 'data-id': infoUid, 'class':"action icon-copy copy", title:tr('Copy value', 'account')}) );
		controlEl.append( $('<span></span>', {'class':'action icon-move-vertical grab' + (this.isEdit() ? '' : ' hidden'), title:tr('Drag up or down', 'account')}) );
		controlEl.append( $('<span></span>', {id: 'changeInfoType-' + infoUid, 'data-id': infoUid, 'class':'action icon-edit' + (this.isEdit() ? '' : ' hidden'), title:tr('Change info type', 'account') }) );
		controlEl.append( $('<span></span>', {id: 'deleteInfo-' + infoUid, 'data-id': infoUid, 'class':"action delete icon-cancel" + (this.isEdit() ? '' : ' hidden') , title:tr('Delete info', 'account')}) );
		valueEl.append(createValueSpan(info.Key, info.value, info.type, infoUid) ).append(controlEl);
		var trEl = $('<tr></tr>', {id: 'info-' + infoUid, 'data-id': info.id, 'data-infoUid': infoUid, 'data-typeid': info.type});
		trEl.append(keyEl).append(valueEl);
	
		infoTable.append(trEl /*infoHtml*/);
		//update password strength meter (if any === if type is password)
		updatePassStrength( $('#meter-' + infoUid), info.value );

		if (this.isEdit())
		{
			makeInfoEditable(infoUid);
		}
		
		if (typeof isMute === 'undefined' || !isMute)	//if isMute is true do not set account's changed property
		{
			this.setChanged(true);	//any new info will change account
			$('#k-' + infoUid).inLineEdit('input').focus().select();
		}
		
		//rtl support: change float:right to float:left (change actions alignment)
		if ($('#account-info').css('direction') == 'rtl')
			$('#account-info > tbody .absFright').removeClass('absFright').addClass('absFleft').css('left', '10px').css('right', 'inherit');
		
	}
	
	function deleteInfoClicked()
	{
		var id = $(this).attr("data-id");
		that.deleteInfoByUid(id);
	}
	
	function copyValueClicked()
	{
		var id = $(this).attr("data-id");
		that.copyInfoValueByUid(id);
	}
	
	this.deleteInfoByUid = function(id)
	{
		this.setChanged(true);
		$("#info-" + id ).fadeOut(200).promise().then(function() {
			$(this).remove();
		});
	}
	
	this.copyInfoValueByUid = function(id)
	{
		that.onCopy($('#v-' + id).text());
	}
	
	this.account = /*obj*/ function() 
	{
		var acc = {	id : 		$('#account').attr('data-id'),
					title: 		$('#account-title').inLineEdit('value'),
					//created: 	$('#created').text(),	//NOTE: date is read-only
					infoPairs:	getInfoPairs(),
					labels: 	$("#lblSelect").val(), 
				};
		
		return acc;
	}
	
	function getInfoPairs()
	{
		var infoKeyValue = [],
		infoCount = 0;
		$('#account tr').each(function() {
			infoCount++;
			info = $('td', this);
			key = info.first().find('.infoKey').inLineEdit('value').toString(); 
			value = info.last().find('.infoValue').inLineEdit('value').toString();
			type = Number($(this).attr('data-typeid'));
			i = newInfo(key, value, type);
			i.id = $(this).attr('data-id');
			i.order = infoCount;
			infoKeyValue.push(i);
		});
		return infoKeyValue;
	}
	
	this.validate = function()
	{
		var valid = true;
		$('#account tr td .inlineInput:invalid').each( function()
		{
			if ($(this).prev().hasClass('infoKey'))
			{
				$(this).effect("highlight").focus();
				valid = false;
			}
		});
		
		return valid;
	}
	
	this.clear = /*void*/ function()
	{
		$('#account-title').removeAttr('data-explicitedit');
		$('#account-title').text('title');
		$('#created').text('creation date');
		$('#account').attr('data-id', '-1');
		//remove info
		$('#account-info > tbody').empty();
		//cancel editables too (only for title,date and desc)
		$('#account-title').inLineEdit('destroy');
		//$('#created').inLineEdit('destroy');
		$('#account-description').inLineEdit('destroy');		
	}
	
	this.startEdit = /*void*/ function()
	{
		if (!this.isVisible())
			return false;	//if form is not displayed
		
		setupFormAsEdit();		//re-setup display form as edit
		m_state = 'edit';
		
		
		var $labels = $('<select id="lblSelect" multiple="" style="width:215px"></select>');
		var lblArr = $('#labels').text().split(', ');
		for (var i = 0; i < lblArr.length; i++)
		{
			var tmplbl = lblArr[i].trim();
			if (tmplbl == '')
				continue;
			$labels.append($('<option></option>', {selected:"true", text: lblArr[i]} ) );
		};
		$labels.append($('<option></option>', {text: tr('Social', 'labels') } ) );
		$labels.append($('<option></option>', {text: tr('Email', 'labels')} ) );
		$labels.append($('<option></option>', {text: tr('Bank', 'labels')} ) );
		$labels.append($('<option></option>', {text: tr('Note', 'labels')} ) );
		$labels.append($('<option></option>', {text: tr('Network', 'labels')} ) );
		$labels.append($('<option></option>', {text: tr('Personal', 'labels')} ) );
		$labels.append($('<option></option>', {text: tr('Work', 'labels')} ) );
		$('#labels').replaceWith($labels);
		$('#lblSelect').select2({
		    tags: true,
		    placeholder: tr("Add labels", 'account'),
		    tokenSeparators: [','].concat( tr('seperators', 'labels') )
    	}).on("change", function(e) {
    		that.setChanged(true);
    	} );

		xEditable('#account-title');	//create inline edit (or update if already created)
		$('#account-title').inLineEdit().inLineEdit('input').change(function() { $('#account-title').attr('data-explicitedit', '1'); that.setChanged(true); });
		
		$('#account-info').addClass('edit-account');
		makeInfoSortable();
		
		//make actions visible
		$('span[id^="changeInfoType-"]').removeClass('hidden');	//make edit controls visible for each info-key pairs
		$('span[id^="deleteInfo-"]').removeClass('hidden');		//make edit controls visible for each info-key pairs
		$('.icon-move-vertical').removeClass('hidden');		//make edit controls visible for each info-key pairs
		$('#account tr').each(function() {
			makeInfoEditable( $(this).attr('data-infoUid') );
		});
		
		return true;
	}
	
	function makeInfoEditable(infoUid)
	{
		//setup actions
		$('#changeInfoType-' + infoUid).click( changeTypeClicked );
		$('#deleteInfo-' + infoUid).click( deleteInfoClicked );
		$('#copy-' + infoUid).click( copyValueClicked );

		xEditable('#k-' + infoUid);
		fixSortableBug( $('#k-' + infoUid).inLineEdit('input') );
		makeValueEditable(infoUid);
	}
	
	function makeValueEditable(infoUid)
	{
		xEditable('#v-' + infoUid);
		//$('#v-' + infoUid).inLineEdit('input').attr('dir', 'auto');	//make value direction auto. example: url in rtl theme
		fixSortableBug( $('#v-' + infoUid).inLineEdit('input') );
		var infoType = $('#info-' + infoUid).attr('data-typeid');
		if (infoType == KEYTYPE_PASSWORD)		//update password strength meter on input change
		{
			updatePassStrength( $('#meter-' + infoUid), $('#v-' + infoUid).inLineEdit('value') );
			$('#v-' + infoUid).inLineEdit('input').on('input', function() {  updatePassStrength( $('#meter-' + infoUid), $(this).val() );  });
		}
		setupAutoTitleFill(infoUid);//automatic fill title for new account
	}
	
	var setupAutoTitleFill = function(infoUid)
	{
		//automatic fill title for new account or accounts with empty title
		if ($('#account').attr('data-id') != "-1" && $('#account-title').inLineEdit('value') != '' )
			return;
		
		$info = $('#v-' + infoUid);
		$username = $('#account tr:not([data-typeid=1])').first().find('td').last().find('.infoValue');
		$url = $('#account tr[data-typeid=4]').first().find('td').last().find('.infoValue');
		if (  $info.is($username) || $info.is($url) )
		{
			$info.inLineEdit('input').change(function()
			{
				//account has been changed
				that.setChanged(true);
				if ($('#account-title').attr('data-explicitedit'))
					return;	//do not change title if user has explicitly changed it.
				var $url = $('#account tr[data-typeid=4]').first().find('td').last().find('.infoValue'), url,
				$user = $('#account tr:not([data-typeid=1])').first().find('td').last().find('.infoValue'), user;
				//get values and validate
				if ($url)
					url = getLocation($url.inLineEdit('value')).hostname;
				if ($user)
					user = $user.inLineEdit('value');
				if ($user.attr("id") == $url.attr("id"))	//if username input (first not password input) and url input are same
					user = '';
				//create title
				title = suggestAccTitle(user, url);
				$('#account-title').inLineEdit('input').val(title);
			});
		}
	}
	
	//FIX: sortable bug: inputs in sortable are not clickable!
	//http://stackoverflow.com/questions/13898027/jquery-ui-sortable-cant-type-in-input-fields-with-cancel-method
	var fixSortableBug = function(item)
	{
		item.on('click', function(e) {
			$(this).trigger({
				type: 'mousedown',
				which: 3
			});
			e.stopPropagation();
		});

		item.on('mousedown', function(e) {
			if(e.which == 3){
				$(this).focus();
			}
			e.stopPropagation();
		});
	}
	
	this.cancelEdit = function() 
	{
		if (!this.isVisible())
			return false;	//if form is not displayed or already in show mode
		//if (this.isShow())
		//	return true;	//already in show mode
		
		setupFormAsShow();
		m_state = 'show';
		$('#account-info').removeClass('edit-account');
		$('#account-title').inLineEdit('destroy');
		//$('#created').inLineEdit('destroy');
		$('#account-description').inLineEdit('destroy');		
		//BUG: info pairs will be cleared and added every time, so ignore them -> what about when canceling edit (bug)
		disableInfoSortable();		
		
		var $labels = $("#lblSelect");
		if ($labels.length > 0)
		{
			var labels = ($labels.val() || []).join(', ');
			$labels.select2('destroy'); 
			$labels.replaceWith('<span id="labels">' + labels + '</span>');
		}

		//FIX: sortable bug: inputs in sortable are not clickable!
		//http://stackoverflow.com/questions/13898027/jquery-ui-sortable-cant-type-in-input-fields-with-cancel-method
		$('#account-info input').unbind('click').unbind('mousedown');
	}
	

	this.setChanged = function(changed)
	{
		changeEvent = (m_isChanged != changed);
		m_isChanged = changed;
		if (changeEvent && typeof(this.onAccountChange) === "function")
			this.onAccountChange(changed);
	}
	
	var setupFormAsShow = function()
	{
		$( "#account" ).dialog({
			autoOpen: false,
			modal: true,
			beforeClose: that.beforeClose,
			//draggable: false,
			buttons: [{text:tr('Edit', 'account'), click: that.onEdit},
					 {text: tr('Close', 'account'), click:that.onOk}],
			open: function() {
				$(this).closest('.ui-dialog').find('.ui-dialog-buttonpane button:eq(1)').focus(); //set default button to Close
			},
			close: function(){m_state = 'closed'; that.clear();},
			width: '450px',
			show: {
				effect: "clip",
				duration: 500
			},
			hide: {
				effect: "clip",
				duration: 400
			}
		});	
	}
	
	var setupFormAsEdit = function()
	{
		$("#account").dialog({
			autoOpen: false,
			modal: true,
			beforeClose: that.beforeClose,
			//draggable: false,
			buttons: [{text:tr('New Info', 'account'), click: that.onNewInfo},
					 {text: tr('Save', 'account'), click: that.onSave},
					 {text: tr('Close', 'account'), click: that.onCancel}] ,
			close: function(){m_state = 'closed'; that.clear();},
			open: function() {
				$(this).closest('.ui-dialog').find('.ui-dialog-buttonpane button:eq(2)').focus(); //set default button to Close
			},						
			width: '450px',
			show: {
				effect: "clip",
				duration: 500
			},
			hide: {
				effect: "clip",
				duration: 400
			}
		});
	}
	
	
	var fillAccount = function(acc)
	{
		$('#account-title').text(acc.title);
		$('#created').text( locale.toLocalDateStr(new Date(acc.created)) );
		$('#labels').text( acc.labels.join(', ') );
		$('#account').attr('data-id', acc.id);

		for (var i = 0; i < acc.infoPairs.length; i++)
		{
			that.addInfo(acc.infoPairs[i], true);
		}
		//rtl support: change float:right to float:left
		//if ($('#account-info').css('direction') == 'rtl')
		//	$('#account-info > tbody .absFright').toggleClass('absFleft');
	}
	
	function xEditable(selector, inputClass)
	{
		$(selector).inLineEdit().inLineEdit('input').change(function() { that.setChanged(true); });
	}

	function changeTypeClicked()
	{
		var changeIconEl = $(this);	//change type icon
		var uid = changeIconEl.attr("data-id");
		var oldType = $('#info-' + uid).attr('data-typeid');
		var typeMenu = $('<div class="pure-menu pure-menu-open"> <a class="pure-menu-heading">' + tr("Choose info type", 'account') + '</a><ul class="menuItems"> <li><a data-typeid="0" class="typeOption" href="#">' + tr('Text', 'keytype') + '</a></li> <li><a data-typeid="1" class="typeOption" href="#">' + tr('Password', 'keytype') + '</a></li> <li><a data-typeid="2" class="typeOption" href="#">' + tr('Note', 'keytype') + '</a></li> <li><a data-typeid="3" class="typeOption" href="#">' + tr('Email', 'keytype') + '</a></li> <li><a data-typeid="4" class="typeOption" href="#">' + tr('URL', 'keytype') + '</a></li>		<li><a data-typeid="5" class="typeOption" href="#">' + tr('Number', 'keytype') + '</a></li> </ul></div>').popup(changeIconEl);
		typeMenu.find("[data-typeid='" + oldType + "']").addClass('pure-menu-active');
		typeMenu.slideDown();
		$('.typeOption').click(function(){
			var selectedTypeId = $(this).attr('data-typeid');
			changeInfoType(uid, selectedTypeId);
			typeMenu.popup('close');
			return false;	//dont change url because of href="#"
		});
	}
	
	function changeInfoType(infoId, typeId)
	{
		that.setChanged(true);
		//uid = $(this).attr("data-id");
		$('#info-' + infoId).attr('data-typeid', typeId);
		//$('#v-' + infoId).attr('data-type', infoTypeName(typeId));	
		$('#meter-' + infoId).remove();				//remove password strength meter (if any)
		$('#v-' + infoId).inLineEdit('destroy');	//remove editable
		$('#v-' + infoId).replaceWith( createValueSpan('TODO:', $('#v-' + infoId).text(), typeId, infoId) );
		makeValueEditable(infoId);
		$('#v-' + infoId).inLineEdit('input').focus();
	}

	function updatePassStrength($meter, pass)
	{
		var passStrengthPercent = passStrength(pass);

		$meter.children().css( {"background-color": rainbow.colourAt(passStrengthPercent),
								"width": passStrengthPercent + '%'} );
	}
	
	function createValueSpan(key, value, type, uid)
	{
		//NOTE: pre is ued instead of span because span displays multiple spaces aa single one!
		//TODO: use key in placeholder ?
		var $v = $('<pre></pre>', {'class':'infoValue', 'data-type': infoTypeName(type), 'data-title': tr('Enter value','account'), id: 'v-' + uid, 'data-maxwidth': '200', text: value});
		if (type == KEYTYPE_PASSWORD)
			return $v.add( $('<div id="meter-' + uid + '" class="progress progress-line"><div data-percentage="0%" class="progress-bar" role="progressbar" aria-valuemin="0" aria-valuemax="100"></div></div>') );
		else
			return $v;
	}
	
	var getLocation = function(href)
	{
		var l = document.createElement("a");
		l.href = href;
		return l;
	};
	
	
	var fixHelperModified = function(e, tr) {
		var $originals = tr.children();
		var $helper = tr.clone();
		$helper.children().each(function(index)
		{
			$(this).width($originals.eq(index).width())
		});
		return $helper;
	};

	function makeInfoSortable()
	{
		$("#account tbody").sortable({
			helper: fixHelperModified ,
			update: function( event, ui ) {that.setChanged(true); ui.item.removeClass('grabbed');},
			//activate: function( event, ui ) {ui.item.addClass('grabbed');},
		}).disableSelection();
		
		$('#account-info.edit-account > tbody > tr').mousedown(function(){ $(this).addClass('grabbed')});
		$('#account-info.edit-account > tbody > tr').mouseup(function(){ $(this).removeClass('grabbed')});
	}

	function disableInfoSortable()
	{
		try {
			$("#account tbody").sortable("destroy");
		}catch(err){}
	}
	
	function getNextUID()
	{
		return ++globalId;
	}	
	
}

