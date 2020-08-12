var locale;
(async function(){
	var selectedLabel = '';
	var findQuery = '';				/*!< holds latest search query >*/
	var findCount = 8;				/*!< holds number of accounts being retreived for every query. NOTE: this is set relative to account list height (in resize event) >*/
	var totalQueryResults = 0;		/*!< holds number of total accounts avaiable for findQuery >*/
	var displayingAccountCount = 0;	/*!< holds numbers accounts displaying in list >*/
	var isFirstLogin = true;
	var notifyTimer;
	// wait to webchannel to establish
	await JakilidUiWaiter

	setupJakilidClient('js/', onError, window.location.hash.substr(1));	//prepare jakilid client, on failure (jakilid not installed) show error
	var AccountView = new AccountViewer();
	AccountView.onSave = saveAccount;
	AccountView.onNewInfo = addNewInfo;
	AccountView.onCancel = function() {AccountView.close();};//cancelAccountEdit;
	AccountView.beforeClose = onAccountClose;
	AccountView.onCopy = function(txt) { JakilidUI.copyToClipboard(txt);};

	JakilidWrapper.onLock = lock;
	JakilidWrapper.onUnlock = onUnlock;
	JakilidWrapper.onError = onError;

	$(window).resize(function() {
		var h = Math.max(document.documentElement.clientHeight, window.innerHeight || 0);
		accountList = $(".MainContainer");
		accountList.height(h-220);	//180
		findCount = Math.ceil(((h-220)/70)+3);	//always find 3 more accounts than how many account which could fit accountList height
		//accountList.mCustomScrollbar("update"); //NOTE: this is not required in version 3
	});


	$(function() {

		/********* everything loaded ************/

		//we load locale here because we need dom to be fully loaded so we could translate it
		locale = new i18n( {localePrefix:'locale/',
							isDebug:1,
							callBack: translatUi,
							"locale": JakilidUI.preferredLanguage || 'en-US'
							} );

		setupUI();

		if (JakilidWrapper.isReady())
			initUI();
		else
			JakilidWrapper.onReady = initUI;			
	});



	function translatUi()
	{
		//wizard window
		// if (typeof isWizard !== 'undefined' && isWizard == true)
		// {
			////user has changed language from wizard, translate wizard welcome page
			// startWizard();
		// }
		//if (locale)	//translatUi would get called in i18n constructor first time where locale has not been set yet so ignore first time although locale's currentLanguage would be JakilidUI.preferredLanguage at first execution of this function
		JakilidUI.setPreferredLanguage(locale.currentLanguage());	//save preferredLanguage for ui
		//wizard
		if (typeof wizard !== 'undefined')
			wizard.translateWizard();
		//login window
		$('#login').attr('title', tr('Unlock Jakilid'));
		//search results
		updateAccCount();
		$('#lblPass').tr('Password');
		//title bar
		$('title').tr('Jakilid Password Manager');
		$('#jakilid').tr('Jakilid');
		$('#lockBtn').attr('title', tr('Lock (Ctrl+L)'));
		$('#configBtn').attr('title', tr('Configurations'));
		$('#minimizeBtn').attr('title', tr('Minimize'));
		$('#maximizeBtn').attr('title', tr('Maximize'));
		$('#quitBtn').attr('title', tr('Quit'));
		$('#aboutBtn').attr('title', tr('About Jakilid'));
		//toolbar
		$('#query').attr('placeholder', tr('Search...'));
		$('#searchBtn').attr('title', tr('Search'));
		$('#cancelSearchBtn').attr('title', tr('Clear results'));
		$('#dismiss').tr('dismiss');
		$('#addAcount').attr('title', tr('Add new item (Ctrl+N)'));
		//account window
		$('#account').attr('title', tr('Item Details','account'));
		$('#account-title').attr('data-title', tr('Enter title','account'));
		$('#lblCreated').tr('Created:');
		$('#lblLabels').tr('Labels:');
		//configurations window
		$('#configwindow').attr('title', tr('Configurations'));
		$('#lblTimeout').tr('How many minutes stay inactive before locking up?', 'config');
		$('#changeMasterPass').tr('Change Master Password', 'config');
		$('#lblGeneral').tr('General Settings', 'config');
		$('#lblDatabase').tr('Database Settings', 'config');
		$('#lblLang').tr('Choose your language:', 'config');

		try
		{
			$('#login').dialog('option', 'title', tr('Unlock Jakilid'));
			$('#login').dialog('option', 'buttons', [{text:tr('login'), click: auth}] );
		} catch(e) { /* login dialog has not been created yet */ }

		//Update scrollbar direction
		//NOTE: this is required only when page direction has been changed.
		$('.MainContainer').mCustomScrollbar("destroy");
		createScrolls();
		
		$('#aboutH1').tr('Jakilid');
		$('#websiteStr').tr('website');
		$('#licStr').tr('licensing information');
		$('#feedbackStr').tr('feedback');
		$('#copyright').tr('made with ❤');

	}

	function createScrolls()
	{
		$(".MainContainer").mCustomScrollbar({
			theme:"dark",
			snapAmount: 71,
			advanced:{
				updateOnContentResize: true,
				updateOnBrowserResize: true,
				autoScrollOnFocus: true,
				contentTouchScroll: true
			},
			mouseWheel:{ normalizeDelta: false },
			callbacks:{
				onTotalScrollOffset: 71*2,
				onTotalScroll: function(){findMore();}
			}
		});
	}

	function setupUI()
	{
		/****** title bar *******/
		if (isBrowser())
		{
			//ui is web browser disable some functionality
			$('#quitBtn').addClass('hidden');
			$('#maximizeBtn').addClass('hidden');
			$('#minimizeBtn').addClass('hidden');
		}
		$('#aboutBtn').click(showAbout);
		$('#lockBtn').click(function(){JakilidWrapper.lock();});
		$('#minimizeBtn').click(function(){JakilidUI.minimize();});
		$('#quitBtn').click(function(){ JakilidUI.quit(); });
		$('#maximizeBtn').click(function()
		{
			if (JakilidUI.isMaximized)
			{
				JakilidUI.restore();
				$(this).removeClass('icon-restore').addClass('icon-expand').attr('title', tr('Maximize'));
			}
			else
			{
				JakilidUI.maximize();
				$(this).removeClass('icon-expand').addClass('icon-restore').attr('title', tr('Restore Down'));
			}
		});
		if (JakilidUI.isMaximized)
		{
			$('#maximizeBtn').removeClass('icon-expand').addClass('icon-restore').attr('title', tr('Restore Down'));
		}


		/******** setup toolbar ************/
		$('#addAcount').click( newAccount);
		$('#dismiss').click(dismissNotify);
		//setup login form action
		$( "#login-form" ).submit(function( event )
		{
			event.preventDefault();
			auth();
		});
		$('#configBtn').click(showConfig);
		$( "#configform" ).submit(function( event )
		{
			event.preventDefault();
			saveConfig();
		});
		//setup search action
		$("#query").val('');
		$( "#searchForm" ).submit(function( event )
		{
			event.preventDefault();
			findAccounts($("#query").val());
		});
		$('#searchBtn').click(function(){
			findAccounts($("#query").val());
		});
		$('#cancelSearchBtn').click(function(){
			$("#query").val('');
			findAccounts();
		});
		
		$('.tab').click(onLabelTabClick);

		//auto seach while typing after 200ms delay
		//BUG: press down any key then press tab search doesn't work. FIX: bind change event too
		var tmrTypeDelay = 0;
		$('#query').keyup(function(e) {
			clearTimeout(tmrTypeDelay);
			tmrTypeDelay = setTimeout(function() {
				findAccounts($("#query").val());
			}, 200);
		});


		/********* setup windows ************/
		//change password
		$('#changeMasterPass').click( function() {
			//translate window
			$('#changePassWin').attr('title', tr('Change Master Password','config'));
			$('#lblChangePass').tr('Change Master Password', 'config');
			$('#lblCurPass').tr('Current Password', 'config');
			$('#curPassword').attr('placeholder', tr('Type current password', 'config'));
			$('#lblNewPass').tr('New Password', 'config');
			$('#newPassword').attr('placeholder', tr('Type New Password', 'config'));
			$('#lblNewPass2').tr('Confirm New Password', 'config');
			$('#newPassword2').attr('placeholder', tr('Type New Password Again', 'config'));

			$('#changePassWin').dialog({
				modal: true,
				autoOpen: true,
				width: '360',
				resizable: false,
				buttons: [{text: tr("OK", 'config'), click: tryMasterPassChange},
						{text: tr("Cancel", 'config'), click: function(){ $(this).dialog("close"); }}] ,
				show: {
					effect: "fade", //"puff", "size",
					duration: 250
				},
				hide: {
					effect: "fade",
					duration: 150
				},
				close: function (event, ui) {
					$('#curPassword').val('');
					$('#newPassword').val('');
					$('#newPassword2').val('');
					$('#errorSpan').text('');
					$('#errorMessage').addClass('invisible');
				}
			});
			/*
			message({msg:'<form class="pure-form pure-form-stacked">' +
						'<fieldset>' +
						'<legend>' + tr('Change Master Password', 'config') + '</legend>' +
						'<label for="curPassword">' + tr('Current Password', 'config') + '</label>' +
						'<input id="curPassword" type="password" placeholder="' + tr('Type current password', 'config') + '">' +
						'<label for="newPassword">' + tr('New Password', 'config') + '</label>' +
						'<input id="newPassword" type="password" placeholder="' + tr('Type New Password', 'config') + '">' +
						'<label for="newPassword2">' + tr('Confirm New Password', 'config') + '</label>' +
						'<input id="newPassword2" type="password" placeholder="' + tr('Type New Password Again', 'config') + '">' +
						'</fieldset>' +
						'</form>' +
						'<div id="errorMessage" class="invisible"><span class="icon-notification" style="color:red"></span> <span id="errorSpan"></span></div>',
						isForm: true,
						buttons: buttonObj
					,
					title:tr('Change Master Password','config')});
			*/
		});
		
		$( "#login" ).dialog({
			autoOpen: false,
			//draggable: false,
			width: 360,
			closeOnEscape: false,
			buttons: [{text:tr('login'), click: auth}] ,
			open: function(event, ui) { $(".ui-dialog-titlebar-close", ui.dialog).hide(); },
			show: {
				effect: "clip",
				duration: 750
			},
			hide: {
				effect: "explode",
				duration: 1000
			}
		});


		/********* setup main window ***********/
		//setup tooltip for main window
		$( '#windowContainer, #title-bar' ).tooltip({	//BUG: can not set tooltip for whole document because it has conflict with editable (change type popup mode editable)
		  show: {
			effect: "slideDown",
			delay: 750
		  }
		});
		//hot keys
		$(window).keydown(function(e)
		{
			if (e.ctrlKey && e.keyCode === 70 )	//Ctrl+F = focus search
			{
				$('#query').focus();
			}
			if (e.ctrlKey && e.keyCode === 78 )	//Ctrl+N = new item
			{
				if ($("#login").dialog( "isOpen" ) /*!JakilidWrapper.isUnlocked()*/)
					return;
				newAccount();
			}
			if (e.ctrlKey && e.keyCode === 76 )	//Ctrl+L = lock
			{
				JakilidWrapper.lock();
			}
			//TODO: add more keyboard shortcuts Ctrl+S, etc
		});
		//ask user to confirm window close when some account has been changed
		window.onbeforeunload = function (e)
		{
			if (!AccountView.isChanged())
				return;
			e = e || window.event;

			// For IE and Firefox prior to version 4
			if (e) {
				e.returnValue = tr("Are you sure you want to discard changes?");
			}

			// For Safari
			return tr("Are you sure you want to discard changes?");
		};
		//set height of account list
		$(window).resize();
		//set custom scrollbar for account's list
		createScrolls();
	}

	function onLabelTabClick()
	{
		selectTab($(this));
	}

	function selectTab(jEl)
	{
		if (jEl.hasClass('active'))
			return;

		$('.tab').removeClass('active');			
		jEl.addClass('active');

		var tab = jEl.attr('data-label');
		// if (jEl.is(':first-child'))		//all
		// 	tab = '';
		// else
		// 	tab = jEl.text();
			
		findAccounts(findQuery, tab);
	}

	function initUI()
	{
		JakilidWrapper.getAvailableProfiles(function(profilesArray) {
			if ((!profilesArray || profilesArray.length == 0))
			{
				if (isBrowser())
				{
					//NOTE: for creating profiles we need to access to OS which is only available using Jakilid browser not user's web browser
					message({msg:tr('You haven\'t configured Jakilid yet. Please run Jakilid from your desktop shortcut or Start menu and configure it.','wizard'),title:tr('Jakilid is not configured','wizard'), buttons:{},icon:'info'});
					return;
				}
				i18n.addScript('js/wizard.js', function(){ wizard.startWizard(startUi); });
			}
			else
				startUi();
		});
		JakilidWrapper.onReady = null;
	}

	function startUi()
	{
		//ui starts with login
		JakilidWrapper.isUnlocked(function(nret){
			if (nret)
				onUnlock();
			else
				$( "#login" ).dialog( "open" );
		});
	}

	function onError()
	{
		freez();	//hide all windows, disable functionalities
		message({msg:tr('Please install and run Jakilid.\nVisit https://github.com/ferrriii/Jakilid for downloading Jakilid Password Manager.'),title:tr('Jakilid is not running'), buttons:{},icon:'alert'});
	}

	function freez(isUnlocked)
	{
		$('.ui-widget-overlay').hide();
		$('.ui-dialog').hide();
		$('#windowContainer').hide();
		if (!isUnlocked)
		{
			$('#lockBtn').hide();
			$('#configBtn').hide();
		}
	}

	function lock()
	{
		JakilidWrapper.lock();
		//if (AccountView.isVisible())
		//{
			//NOTE: single selector is faster than multiple selectors. http://stackoverflow.com/questions/17752874/multiple-selector-vs-single-selector-performance
			$('.ui-widget-overlay').hide();
			$('.ui-dialog').hide();	//hide all dialogs (account, message box)
		//}
		$('#windowContainer').hide();
		$( "#login" ).dialog("open");
		$('#lockBtn').hide();
		$('#configBtn').hide();
		$('[id^=popup]').trigger('closePopup');	//close change type popup menu
		if ($( "#about" ).hasClass('ui-dialog-content'))	//if about dialog has been created and is open, close it
			$( "#about" ).dialog("close");
	}

	function unLockUi()
	{
		if (AccountView.isVisible())
			$('#account').parent().show();	//display account dialog
		$('.message-dialog').parent().show();	//display message box dialoges (if any)
		try
		{ 	//configuration is not setup at startup
			if ($("#configwindow").dialog( "isOpen" ))
				$('#configwindow').parent().show();	//display configuration dialog
		} catch(e) {}


		$('.ui-widget-overlay').show();			//display modal dialog overlays (if any)
		$('#windowContainer').show('slow');		//display main window contents
		$("#login").dialog( "close" );		//hide authentication form
		$('#lockBtn').show();
		$('#configBtn').show();
	}

	function onUnlock()
	{
		unLockUi();
		JakilidWrapper.getCurrentProfile(function(prof) {
			locale.changeTranslation( prof.lang );
		});

		if (isFirstLogin)
		{
			JakilidWrapper.upgradeDb(upgradeResults );

		}
		isFirstLogin = false;
	}

	function fetchInitialData()
	{
		updateLabels();
		findAccounts();
	}

	function updateLabels()
	{
		JakilidWrapper.labels(function(labels)
			{
				var tabs = $('.tab'),
					tabsContainer = $('#tabs'),
					newTab;
				for (var i = 0; i < labels.length; i++)
				{
					var lbl = labels[i];
					var existingTab;
					if (i+1 < tabs.length)
					{
						existingTab = $(tabs[i+1]);
						if (existingTab.text().toLowerCase() != lbl.toLowerCase())
						{
							//change tab label
							existingTab.text(lbl);
							existingTab.attr('data-label', lbl);
						}
					}else
					{
						//add new label tab
						existingTab = $( '<div></div>', {'class':'tab', text: lbl, 'data-label': lbl, click: onLabelTabClick } );
						tabsContainer.append(  existingTab  ) ;
					}
				};
				//remove extra tabs
				for (var i = labels.length+1; i < tabs.length; i++)
				{
					$(tabs[i]).remove();
				};

				//make selected tab active
				var selectedTab = $('.tab[data-label="' + selectedLabel + '"]');
				if (selectedTab.length <= 0)	//previously selectect tab has been removed
					selectedTab = $('.tab[data-label=""]');

				selectTab(selectedTab);

			});
	}



function upgradeResults(nret)
	{
		switch(nret)
		{
			case 0: 	//failed to upgrade
				freez(true);	//disable view/edit/search accounts
				isFirstLogin = true;	//force to try upgrade on unlock
				var buttons = {};
				buttons[tr('Retry')] = function() { $(this).dialog("close"); JakilidWrapper.upgradeDb(upgradeResults); };
				buttons[tr('Visit Jakilid download page')] = function() { 
				     form = document.createElement("form");
				     form.method = "GET";
				     form.action = "https://github.com/ferrriii/Jakilid";
				     form.target = "_blank";
				     document.body.appendChild(form);
				     form.submit();
				     document.body.removeChild(form);
				};
				message({msg:tr("This database is for an older version of Jakilid. We tried to upgrade it to current version but we failed! Please use latest Jakilid version or try upgrading the database again.", 'upgrade'),
						title: tr('Error while upgrading Database', 'upgrade'),
						icon: 'alert',
						buttons: buttons
						});
				break;
			case 1: 	//successfully upgraded
				notify( tr('Database successfully upgraded to current version', 'upgrade') ,'info');
				fetchInitialData();
				break;
			case 2: 	//no upgrade required
				fetchInitialData();
				break;
			case 3: 	//higher version
				freez(true);	//disable view/edit/search accounts
				isFirstLogin = true;	//force to try upgrade on unlock
				var buttons = {};
				buttons[tr('Visit Jakilid download page')] = function() { 
				     form = document.createElement("form");
				     form.method = "GET";
				     form.action = "https://github.com/ferrriii/Jakilid";
				     form.target = "_blank";
				     document.body.appendChild(form);
				     form.submit();
				     document.body.removeChild(form);
				};
				message({msg:tr("This database is saved using a higher version of Jakilid. Your data are safe but we don't know how they've been saved, Please download and use the latest Jakilid version to access this database properly.", 'upgrade'),
						title: tr('Database Version Mismatch', 'upgrade'),
						icon: 'info',
						buttons: buttons
						});
				break;
		}
	}

	function auth()
	{
		JakilidWrapper.authenticate('default', $('#password').val(), function(nret) {
			$('#loginIndicator').addClass('hidden');
			if (!nret)
			{
				message({
					title: tr("Access Denied"),
					msg: tr("That password was wrong. Please try again."),
					icon: "alert"
				});
			}
		});
		$('#loginIndicator').removeClass('hidden');
		$('#password').val('');
	}
	
	function showAbout()
	{
		JakilidWrapper.about(function(res){
			$('#aboutVer').text( tr('Version %1', [locale.toLocalNumber(res.version)] )); // .arg(  ) );
			$('#aboutBuild').text( tr('Build Date: %1', [locale.toLocalDateStr( new Date(res.buildDate) )] ));// .arg( locale.toLocalDateStr( new Date(res.buildDate) ) ) );
		});
		
		$( "#about" ).dialog({
			autoOpen: true,
			//draggable: false,
			width: 360,
			modal: true,
			title: tr('About Jakilid Password Manager'),
			//closeOnEscape: false,
			buttons: [] ,
			open: function(event, ui) { },
			show: {
				effect: "fade",
				duration: 200
			},
			hide: {
				effect: "fade",
				duration: 200
			}
		});	
	}

	function findAccounts(query, tab)
	{
		if (query == findQuery && selectedLabel == tab)
			return;	//duplicate search (debounce it)

		if (query)
		{
			$('#cancelSearchBtn').removeClass('hidden');
			$('#searchBtn').addClass('hidden');
		}else
		{
			$('#searchBtn').removeClass('hidden');
			$('#cancelSearchBtn').addClass('hidden');
		}

		clearAccountList();	//sets displaying number to 0
		findQuery = query;
		if (tab != undefined)
			selectedLabel = tab;

		findMore();
	}

	function findMore()
	{
		//var queryResults = JakilidWrapper.find(findQuery, findCount, displayingAccountCount);
		//addAccounList(queryResults);
		JakilidWrapper.find(findQuery, selectedLabel, findCount, displayingAccountCount, addAccounList);
	}

	function addAccounList(queryResults)
	{
		totalQueryResults = queryResults.total;
		displayingAccountCount += queryResults.items.length;
		updateAccCount();
		//var accHtml = '';
		var acountList = $("#accountList");
		for (var i = 0; i < queryResults.items.length; i++)
		{
			var acc = queryResults.items[i];

			var accSelectorId =  acc.id.replace(/[{}]/g, '');
			//accHtml += '<li class="acc" id="acc-' + accSelectorId+ '" data-id="' + acc.id + '">' + '<div class="accListItem" id="accItem-' + accSelectorId + '" data-id="' + acc.id + '" style="backrgound-color:#ff3333"><img class="fleft" src="img/user-small.png">' + '<h3>' + htmlEncode(acc.title ? acc.title : tr('No Title') ) + '</h3>' /*+ tagsHtml*/ + '</div><div class="rollOver"><div id="edit-' + accSelectorId + '" data-id="' + acc.id + '" class="editAcc action icon-edit" title="' + tr('Edit item') + '"></div><div id="delete-' + accSelectorId + '" data-id="' + acc.id + '" class="delAcc action delete icon-remove" title="' + tr('Delete item') + '"></div></div>' + '</li>';
			var accLiEl = $('<li></li>', 
						{'class': 'acc',
						 id: 'acc-' + accSelectorId,
						 'data-id': acc.id
						});
			var accDivEl = $('<div></div>', {
							'class': 'accListItem',
							id: 'accItem-' + accSelectorId,
							'data-id': acc.id
							//style:"backrgound-color:#ff3333"
							});
			accDivEl.append( $('<img />', {
								'class': 'fleft',
								src: 'img/user-small.png'
							}));
			accDivEl.append( $('<h3></h3>', {
								text: acc.title ? acc.title : tr('No Title')
							}));	

			var accROverEl = $('<div></div>', {
							'class': 'rollOver'
							});
			accROverEl.append( $('<div></div>', {
								'class': 'editAcc action icon-edit',
								id: 'edit-' + accSelectorId,
								'data-id': acc.id,
								title: tr('Edit item')
							}));
			accROverEl.append( $('<div></div>', {
								'class': 'delAcc action delete icon-remove',
								id: 'delete-' + accSelectorId,
								'data-id': acc.id,
								title: tr('Delete item')
							}));

			accLiEl.append(accDivEl).append(accROverEl);
			acountList.append( accLiEl );
		}
		//$("#accountList").append( accHtml );
		//QUESTION: which is better: $(".accListItem") or $(".acc .accListItem") or $acc = $(".acc"); $acc.find(".editAcc") ?
		//ANSWER: http://gregfranko.com/jquery-best-practices/#/23
		var $acc = $(".acc").off();
		$acc.on("click", ".accListItem", function () { id = $(this).attr("data-id"); showAccountById(id); } );
		$acc.on("click", ".editAcc", function () { id = $(this).attr("data-id"); editAccountById(id); } );
		$acc.on("click", ".delAcc", function () { id = $(this).attr("data-id"); confirmDelete(id); } );
	}

	/*
	function createTagsHtml(acc)
	{
		var tags = findTags(acc);
		var tagsHtml = '<span class="tags">'; //<span class="ui-icon ui-icon-tag InlineBlock"></span>';
		for (var t = 0; t < tags.length; t++)
		{
			tagsHtml += '<span class="tag">' + htmlEncode(tags[t]) + '</span>';
		}
		tagsHtml += '</span>';
		if (tags.length <= 0)	//there's no tag remove tag
			tagsHtml = '';

		return tagsHtml;
	}
	*/

	function updateAccountListItem(acc)
	{
		//check acc validation
		accEl = $('#acc-' + acc.id.replace(/[{}]/g, '') );
		if (accEl.length <= 0)
			return;	//account does not exist!
		titleEl = accEl.find( "h3" );
		titleEl.text(acc.title ? acc.title : tr('No Title'));	//set new title
		/*
		tagsEl = accEl.find(".tags");
		var tagsHtml = createTagsHtml(acc);
		if (tagsEl.length)
		{
			if (tagsHtml != '')
				tagsEl.replaceWith(tagsHtml);	//replace tags
			else
				tagsEl.remove();			//theres' no tags remove olds
		}
		else
			titleEl.after(tagsHtml)		//add new tags
		*/
	}

	function removeAccountFromList(id)
	{
		displayingAccountCount--;
		totalQueryResults--;
		$("#acc-" + id ).fadeOut('slow').promise().then(function() {
			$(this).remove();
			updateAccCount();
		});
	}

	function updateAccCount()
	{
		var cnt = displayingAccountCount;
		var stat;
		if (totalQueryResults <= 0)
		{
			if (findQuery && findQuery != '')
			{
				if (selectedLabel)
					stat = tr('Nothing matched "%1" from "%2" category', [findQuery, selectedLabel]);
				else
					stat = tr('Nothing matched "%1"', [findQuery]);
			}
			else
				stat = tr('There\'s no item to display. Try adding a new one.');
		}
		else
		{
			if (findQuery && totalQueryResults>1)
			{
				if (cnt < totalQueryResults)
				{
					if (selectedLabel)
						stat= tr('Displaying %1 out of %2 items containing "%3" from "%4" category', [locale.toLocalNumber(cnt), locale.toLocalNumber(totalQueryResults), findQuery, selectedLabel ] );
					else
						stat= tr('Displaying %1 out of %2 items containing "%3"', [locale.toLocalNumber(cnt), locale.toLocalNumber(totalQueryResults), findQuery]);
				}
				else
				{
					if (selectedLabel)
						stat= tr('Displaying %1 items containing "%2" from "%3" category', [locale.toLocalNumber(cnt), findQuery, selectedLabel]);
					else
						stat= tr('Displaying %1 items containing "%2"', [locale.toLocalNumber(cnt), findQuery]);
				}
			}
			else if (findQuery && totalQueryResults==1)
			{
				if (selectedLabel)
					stat= tr('Displaying the only item containing "%1" from "%2" category', [findQuery, selectedLabel]);
				else
					stat= tr('Displaying the only item containing "%1"', [findQuery]);
			}
			else if (totalQueryResults>1)
			{
				if ( cnt< totalQueryResults)
				{
					if (selectedLabel)
						stat= tr('Displaying %1 out of %2 items from "%3" category', [locale.toLocalNumber(cnt), locale.toLocalNumber(totalQueryResults), selectedLabel]);
					else
						stat= tr('Displaying %1 out of %2 items', [locale.toLocalNumber(cnt), locale.toLocalNumber(totalQueryResults)]);
				}
				else
				{
					if (selectedLabel)
						stat= tr('Displaying %1 items from "%2" category', [locale.toLocalNumber(cnt), selectedLabel]);
					else
						stat= tr('Displaying %1 items', [locale.toLocalNumber(cnt)]);

				}
			}
			else if (totalQueryResults==1)
			{
				if (selectedLabel)
					stat= tr('Displaying the only item from "%1" category', [selectedLabel] );
				else
					stat= tr('Displaying the only item');
			}

		}
		$('.ResultStat').text(stat);
	}

	function newAccount()
	{
		var acc = {	id : -1,
					title: tr('', 'account'),
					created: new Date(),
					infoPairs:	[
									newInfo(tr('Username', 'account'), '', KEYTYPE_TEXT),
									newInfo(tr('Password', 'account'), '', KEYTYPE_PASSWORD),
									newInfo(tr('Login URL', 'account'), '', KEYTYPE_URL)
									//newInfo(tr('Tags', 'account'), '', KEYTYPE_TAGS)
								],
					labels: [selectedLabel]
				};

		editAccount(acc);
	}

	function showAccountById(id)
	{
		JakilidWrapper.load(id, function(acc) {
			if (!acc || jQuery.isEmptyObject(acc))
			{
				message({msg: tr('There was an error while accessing this item. Please try again.'),
						 title:tr('Failed to show item'),
						 icon: 'alert'
						});
				return false;
			}

			showAccount(acc);
		});
	}

	function showAccount(acc)
	{
		AccountView.show(acc);
	}

	function editAccountById(id)
	{
		 JakilidWrapper.load(id, editAccount);
	}

	function editAccount(acc)
	{
		if (!acc || jQuery.isEmptyObject(acc))
		{
			message({msg: tr('There was an error while accessing this item. Please try again.'),
					 title:tr('Failed to edit item'),
					 icon: 'alert'
					});
			return false;
		}

		AccountView.edit(acc);
	}

	function confirmDelete(id)
	{
		var buttonObj = {};
		buttonObj[tr('Yes, Delete it')] = function() { deleteAccountById(id); $(this).dialog("close");};
		buttonObj[tr('Cancel Delete')] = function() {$(this).dialog("close");};
		message({
			title: tr("Confirm Delete"),
			msg: tr("You will not be able to restore this item, are you sure?"),
			buttons: buttonObj,
			defaultBtn: 1 /* cancel delete*/,
			icon: "alert"
		});

		buttonObj = null;
	}

	function deleteAccountById(id)
	{
		JakilidWrapper.remove(id, function(nret) {
			if (nret)
			{
				updateLabels();
				removeAccountFromList(id.replace(/[{}]/g, ''));
			}else
				message({msg: tr('There was an error while accessing this item. Please try again.'),
						 title:tr('Failed to delete item'),
						 icon: 'alert'
						});
		});
	}

	function saveAccount()
	{
		if (!AccountView.isChanged())
		{
			AccountView.close();
			//Warning: display warning?
			return;
		}
		if (!AccountView.validate())
			return;

		var acc = AccountView.account();
		//var accId = JakilidWrapper.save( acc );
		JakilidWrapper.save( acc, function(accId){
			if (accId != '')
			{
				AccountView.setChanged(false);
				AccountView.close();
				updateAccountListItem(acc);
				notify(tr('Data were saved. Click here to view it.'), 'success', function(){showAccountById(accId);} );
				//update label tabs
				updateLabels();
				updateSearchResults(accId);
			}
			else
				message({msg: tr('There was an error while saving data. Please try again.'),
						 title:tr('Failed to save item'),
						 icon: 'alert'
						});
		});

		//acc = null;
	}

	function updateSearchResults(accId)
	{
		//find current query results again and check if new added account (acc) is in results, if so update displaying accounts
		var count = Math.ceil( (displayingAccountCount / findCount) ) * findCount;	//round up displayingAccountCount
		if (count <= 0)
			count = findCount;
		//var resultsObj = JakilidWrapper.find(findQuery, count, 0);
		JakilidWrapper.find(findQuery, selectedLabel, count, 0, function(resultsObj){
			for (var i = 0; i < resultsObj.items.length; i++)
			{
				if (resultsObj.items[i].id == accId)
				{
					//found just added account in results, update view
					clearAccountList();
					addAccounList(resultsObj);
					return;
				}
			}
			totalQueryResults = resultsObj.total;
			updateAccCount();	//if just added account is not in current results, update total accounts number (maybe changed) in search results stat
		});
	}

	function onAccountClose(e, ui)
	{
		if (!AccountView.isChanged())
			return;

		e.preventDefault();	//prevent account close

		var buttonObj = {};
		buttonObj[tr('Yes, Discard Changes')] = function() { AccountView.setChanged(false); /* <- avoid catching again */ $(this).dialog("close"); AccountView.close();};
		buttonObj[tr('No, wait!')] = function() {$(this).dialog("close");};
		message({
			title: tr("Data have been changed"),
			msg: tr("Are you sure you want to discard changes?"),
			buttons : buttonObj,
			defaultBtn: 1,
			icon: "question"
		});

		e = null;
		ui = null;
		buttonObj= null;
	}

	function addNewInfo()
	{
		//TODO: according to available info, add new one (tags, note, url,...)
		AccountView.addInfo( newInfo('','', KEYTYPE_TEXT)	);
	}

	function clearAccountList()
	{
		$('#accountList').empty();
		displayingAccountCount = 0;
	}

	/*
	function findTags(acc)
	{
		var tags = [];
		for (var i = 0; i < acc.infoPairs.length; i++)
		{
			if (acc.infoPairs[i].type == KEYTYPE_TAGS ) {
				var value = acc.infoPairs[i].value;//"a,b";//acc.infoPairs[i];
				tags = value.split(",");

				}
		}
		tags = tags.map(Function.prototype.call, String.prototype.trim);	//trim every tag
		return tags;
	}
	*/

	function showConfig()
	{
		//var profile = JakilidWrapper.getCurrentProfile();
		JakilidWrapper.getCurrentProfile(function(profile){
			$('#timeout').val(profile.timeout / 60);	//convert to minutes
			$('#dbAddress').text( tr("Database: %1", 'config', [profile.db])/*.arg(profile.db)*/ );
			$('#configLang').val(profile.lang);

			$( "#configwindow" ).dialog({
				autoOpen: true,
				//draggable: false,
				width: 375,
				modal: true,
				buttons: [{text: tr("OK", 'config'), click: saveConfig},
						{text: tr("Cancel", 'config'), click: function(){ $(this).dialog("close"); }}] ,
				show: {
					effect: "clip",
					duration: 500
				},
				hide: {
					effect: "clip",
					duration: 400
				}
			});

			//profile = null;
		});
	}

	function saveConfig()
	{
		timeout = parseFloat($('#timeout').val());
		lang = $('#configLang').val();
		if (!$('#timeout')[0].checkValidity() || isNaN(timeout) || !isFinite(timeout) || timeout<=0)
		{
			var buttonObj = {};
			buttonObj[tr('OK')] = function() { $(this).dialog("close"); $('#timeout').focus(); };
			message({msg: tr('The value you entered for auto lockout is invalid. Please enter a number between 1 and 999.', 'config'),
					 title: tr('Wrong timeout value', 'config'),
					 icon: 'alert',
					 buttons : buttonObj
					});
			buttonObj = null;
			return;
		}
		JakilidWrapper.setSettings(timeout * 60, lang, function(nret){
			if (!nret)	//convert to seconds
			{
				message({msg: tr('There was an error while saving your settings. Please try again.', 'config'),
						 title:tr('Failed to save settings', 'config'),
						 icon: 'alert'
						});
				return;
			}
			//update translation
			locale.changeTranslation(lang );

			$( "#configwindow" ).dialog("close");
		});
	}

	function tryMasterPassChange()
	{
		var $curPass = $('#curPassword').val();
		var $newPass = $('#newPassword').val();
		var $newPass2 = $('#newPassword2').val();

		if ($curPass == '')
		{
			$('#errorSpan').text(tr('Please enter your current password', 'config'));
			$('#errorMessage').removeClass('invisible');
			$('#curPassword').focus();
			return;
		}

		if ($newPass != $newPass2)
		{
			$('#errorSpan').text(tr('New passwords don\'t match. Try again?', 'config'));
			$('#errorMessage').removeClass('invisible');
			$('#newPassword2').focus();
			return;
		}

		var pasChangeDialog = $(this);
		JakilidWrapper.isPassStrong($newPass, function(err){
			if (err != '')
			{
				$('#errorSpan').text(tr(err,'wizard'));
				$('#errorMessage').removeClass('invisible');
				$('#newPassword').focus();
				return;
			}

			JakilidWrapper.changePass($curPass, $newPass, function(nret){
				if (nret)
				{
					pasChangeDialog.dialog("close");	//close change pass popup form
					message({msg: tr('Your master password was successfully changed.', 'config'), title:tr('Password successfully changed', 'config'), icon: 'info'});
				}else
					message({msg: tr('There was an error while changing master password. Please try again.', 'config'),
						 title:tr('Failed change master password', 'config'),
						 icon: 'alert'
						});
				pasChangeDialog = null;
			});
		});

	}

	function notify(msg, type, onClick)
	{
		var m = $('#messageBox');
		$('#msg').html(msg);
		var c = "info"
		if (type=="success")
			c="success";
		else if (type=="error")
			c="error";
		m.css('display', 'block');
		m.addClass(c);

		if (onClick != null && onClick !== 'undefined')
			m.click(onClick);
		else
			m.off('click');
			m.animate({opacity:1.0}, 'fast', function(){ notifyTimer= setTimeout(dismissNotify, 5000);} );
		m = null;	//javascript closures and leak patterns are confusing so make make everything clear to GC
	}

	function dismissNotify()
	{
		clearTimeout(notifyTimer);
		$('#messageBox').off('click');
		$('#messageBox').animate({opacity:0}, 'fast', function(){$this=$(this); $this.css('display', 'none'); $this.attr('class', ''); } );
	}

	function message( options )
	{
		var title="Message",
		msg,
		//img,
		icon,
		show = {
				effect: "fade", //"puff", "size",
				duration: 250
			},
		hide = {
				effect: "fade",
				duration: 150
			},
		buttons = {},
		defaultButton = '';
		buttons[tr('OK')] = function() { $(this).dialog("close"); };

		if (typeof options === 'object')
		{
			//override defaults
			if (typeof options.title === 'string')
				title = options.title;
			if (typeof options.msg === 'string')
				msg = options.msg;
			if (typeof options.message === 'string')
				msg = options.message;
			//if (typeof options.img === 'string')
			//	img = options.img;
			//if (typeof options.image === 'string')
			//	img = options.image;
			if (typeof options.icon === 'string')
				icon = options.icon;
			if (typeof options.show === 'object')
				show = options.show;
			if (typeof options.hide === 'object')
				hide = options.hide;
			if (typeof options.buttons === 'object')
				buttons = options.buttons;
			if (typeof options.defaultButton === 'number')
				defaultButton = options.defaultButton;
			if (typeof options.defaultBtn === 'number')
				defaultButton = options.defaultBtn;
		}
		if (typeof options === 'string')
			msg = options;

		var openFunc = null;
		if (defaultButton != '' && defaultButton>= 0)
		{
			openFunc = function() {
				$(this).closest('.ui-dialog').find('.ui-dialog-buttonpane button:eq(' + defaultButton + ')').focus();
			}
		}

		var iconEl = $( "<span></span>", {"class": "bigIcon msgIcon"});
		
		if (icon == "alert")
			iconEl.addClass( "icon-notification alert-back" ); //icon = '<span class="bigIcon icon-notification alert-back msgIcon"></span>';
		else if (icon == "question")
			iconEl.addClass( "icon-question question-back" ); //icon = '<span class="bigIcon icon-question question-back msgIcon"></span>';
		else if (icon == "info")
			iconEl.addClass( "icon-info info-back" ); // icon = '<span class="bigIcon icon-info info-back msgIcon"></span>';

		var content = $( "<div></div>", {"class": "message-dialog", "title": title})
			.append(
				$( "<p></p>", {text: msg} )
				.prepend(iconEl)
			);

		content.appendTo('body')
		.dialog({
			modal: true,
			title: title,
			autoOpen: true,
			width: '360',
			resizable: false,
			buttons: buttons,
			show: show,
			hide: hide,
			open: openFunc,
			close: function (event, ui) {
				$(this).remove();
			}
		});
	}

})();
