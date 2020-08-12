
var jakilidAddonUi = function(){

	var matchingAccounts = [];	//holds auto fill suggested accounts
	var state = 0;				//addon display state
	var LOCKED_STATE = 1;
	var UNLOCKED_STATE = 2;
	var FILLAVAILABLE_STATE = 3;
	var DISABLED_STATE = 4;
	var UNCONFIGURED_STATE = 5;
	var idleTimeout;
	var locale;		//translations and locale
	var unLockedStateCache = false;//assume locked
	var _submit;	//holds original htmlform submit function
	JakilidWrapper.onLock = showLocked;
	JakilidWrapper.onUnlock = inspectForSuggestion;
	JakilidWrapper.onError = showDisabled;

	//page forms event handler: suggest saving new accounts
	function onFormSubmit(e)
	{
		e = e || window.event;
		var target = e.target || e.srcElement;	//srcElement is for IE, not really needed for Firefox extension!
		//QUESTION: could target be empty?!

		if (!unLockedStateCache)
			return showLocked();	//locked

		//TODO:prevent session lock because user is active
		var formObj = formFill.formFromElement(target);

		//make sure form has not been auto filled
		if (formObj.autoFilled == '1')
			return;

		//if form is login,signup
		var isLogin = false;
		for (var i = 0; i < formObj.inputs.length; i++)
		{
			isLogin = isLogin || (formObj.inputs[i].type == 'password');
			if (isLogin) break;
		}
		if (!isLogin)	//submitted form is search, newsletter, etc
			return;

		var acc = formFill.accountFromForm(formObj);
		//alert(JSON.stringify(acc) );
		if (acc)
		{
			for (var i = 0; i < matchingAccounts.length; i++)
			{
				var alreadySavedAcc = matchingAccounts[i];
				if (areAccSame(acc,alreadySavedAcc))
					return;	//this acc already has been saved
			}
			askSavingLogin(acc);
		}
	}

	function onPageChanged(isHtmlReady)
	{
		if (unLockedStateCache)
		{
			//TODO:prevent session lock because user is active
			//TODO: do no reinspect if already done
			inspectForSuggestion();
		}else
			showLocked();	//locked
	}

	function auth(e)
	{
		e && e.stopPropagation();	//if there's event (like menu click) stop it from propagation
		JakilidWrapper.authenticate('default', $('#password').val(), function(nret){
			if (!nret)
				alert(tr('Wrong Password!', 'addon'));
			else
			{
				JakilidWrapper.getCurrentProfile(function(prof) {
						locale.changeTranslation( prof.lang );
						idleTimeout = prof.timeout;
					});
			}
		});
		document.getElementById('login').hidePopup();
		$('#password').val('');
	}

	function lock(e)
	{
		e && e.stopPropagation();	//if there's event (like menu click) stop it from propagation
		JakilidWrapper.lock();
	}
	
	function onButtonClick(e)
	{
		//if event available stop propagation
		e && e.stopPropagation();
		
		switch (state)
		{
			case LOCKED_STATE:
				//open login panel
				//document.getElementById('login').openPopup(document.getElementById('PanelUI-button'), 'bottomright topright', -36, 0, false, false);
				document.getElementById('login').openPopup(document.getElementById('navigator-toolbox'), 'bottomright topright', -36, 0, false, false);	
				//document.getElementById('login').openPopup(document.getElementById('PanelUI-button'), 'bottomright topleft', -36, 0, false, false);
				break;
			case UNLOCKED_STATE:
				//open ui
				openJakilid();
				break;
			case DISABLED_STATE:
				//inform user
				alert(tr('Please install and run Jakilid.\nVisit http://Jakilid.com for downloading Jakilid Password Manager.'));
				break;
			case FILLAVAILABLE_STATE:
				//auto fill first available suggestion
				autoFill(0);
				break;
			case UNCONFIGURED_STATE:
				alert(tr("You haven't configured Jakilid yet. Please run Jakilid from your desktop shortcut or Start menu and configure it.", 'wizard'));
				break;
		}
	}
	
	function inspectForSuggestion()
	{
		unLockedStateCache = true;	//NOTE: inspectForSuggestion function should get called only when session is unlocked
		var curUrl = gBrowser.contentWindow.location.href;
		var forms = formFill.pageForms();	//TODO: cache pageForms for further use in autoFill ?
		if (forms.length <= 0)		//is there any form that we could fill it? (there maybe some forms on page but we should be able to fill it)
			return showUnlocked();	//page has no form
		showUnlocked();	//make sure state changes to unlocked view (first time)

		
		JakilidWrapper.findByUrl(curUrl, -1,0, function(objResults){
			var accArray = objResults.items;
			matchingAccounts = accArray;	//cache suggestions
			if (accArray.length <=0)
				return showUnlocked(); //there's no account associated with this page, maybe it's new suggest saving account?
			else
				showAutoFill();	//change addon state to suggestion available
			
			setupSuggestion(accArray);	//prepare suggestions menu
		});
	}

	function openJakilid(e)
	{
		e && e.stopPropagation();	//if there's event (like menu click) stop it from propagation
		/*
		var win = Components.classes['@mozilla.org/appshell/window-mediator;1']
					  .getService(Components.interfaces.nsIWindowMediator)
					  .getMostRecentWindow('navigator:browser');
		win.gBrowser.selectedTab = win.gBrowser.addTab('chrome://Jakilid/content/ui/index.html#' + JakilidWrapper.sid());
		*/
		openAndReuseOneTabPerURL('chrome://Jakilid/content/ui/index.html#' + JakilidWrapper.sid());
	}
	
	function openAndReuseOneTabPerURL(url)
	{
		var wm = Components.classes["@mozilla.org/appshell/window-mediator;1"]
			.getService(Components.interfaces.nsIWindowMediator);
		var browserEnumerator = wm.getEnumerator("navigator:browser");

		// Check each browser instance for our URL
		var found = false;
		while (!found && browserEnumerator.hasMoreElements())
		{
			var browserWin = browserEnumerator.getNext();
			var tabbrowser = browserWin.gBrowser;

			// Check each tab of this browser instance
			var numTabs = tabbrowser.browsers.length;
			for (var index = 0; index < numTabs; index++)
			{
				var currentBrowser = tabbrowser.getBrowserAtIndex(index);
				if (url.toLowerCase() == currentBrowser.currentURI.spec.toLowerCase())
				{
					// The URL is already opened. Select this tab.
					tabbrowser.selectedTab = tabbrowser.tabContainer.childNodes[index];

					// Focus *this* browser-window
					browserWin.focus();

					found = true;
					break;
				}
			}
		}

		// Our URL isn't open. Open it now.
		if (!found)
		{
			var recentWindow = wm.getMostRecentWindow("navigator:browser");
			if (recentWindow)
			{
				recentWindow.delayedOpenTab(url, null, null, null, null);
			}
			else
			{
				// No browser windows are open, so open a new one.
				window.open(url);
			}
		}
	}

	function autoFill(index)
	{
		formFill.fillAcc(matchingAccounts[index]);
		//NOTE: we use cached version of account for form fill but we still load account itself so account's properties such as usage and last-used would be updated
		JakilidWrapper.load(matchingAccounts[index].id, function(){} );
	}

	function setupSuggestion(accArray)
	{
		//accArray is not empty

		var menu = $('#jakilid-button > menupopup');
		menu.prepend( $('<menuseparator />', {id: 'accountSeperator'}) );
		for(var i = accArray.length; i--; )
		{
			menu.prepend( createAutoFillMenu(accArray[i].title, i) );
		}
		
		//var autoFillMenu = '';
		//for (var i = 0; i < accArray.length; i++)
		//{
		//	autoFillMenu += createAutoFillMenu(accArray[i].title, i);
		//}
		//$('#jakilid-button > menupopup').prepend( autoFillMenu + /* '<menuseparator class="autoFillMenu"/>' + detailedMenu + */ '<menuseparator id="accountSeperator"/>' );
	}

	function createAutoFillMenu(title, index)
	{
		return $('<menuitem />', {'class':'autoFillMenu defaultItem',
		'label': tr("Auto fill '%1'", 'addon', [title])/*.arg(title)*/,
		on:{
			command: function(event){
				event.stopPropagation();
				jakilidAddonUi.autoFill(index);
			}
		}});
		//return '<menuitem label="' + tr("Auto fill '%1'", 'addon').arg(title) + '" class="autoFillMenu defaultItem" oncommand="event.stopPropagation(); jakilidAddonUi.autoFill(' + index + '); "/>';
	}

	function askSavingLogin(acc)
	{
		$('#SaveLoginBtn').off('command').bind('command', function(e) {
			e.stopPropagation();
			saveLogin(acc);
			document.getElementById('askSave').hidePopup();
		});

		$('#LoginDetailsBtn').off('command').bind('command', function(e) {
			e.stopPropagation();
			//TODO: implement edit account
			document.getElementById('askSave').hidePopup();
		});

		document.getElementById('askSave').openPopup(document.getElementById('navigator-toolbox'), 'bottomright topright', -36, 0, false, false);
	}

	function clearLoginSave()
	{
		$('#SaveLoginBtn').off('command');
		$('#LoginDetailsBtn').off('command');
	}

	function saveLogin(acc)
	{
		JakilidWrapper.save(acc, function(accId){
			if (accId == '')
				alert(tr('There was an error while saving data. Please try again.'));
		});
	}

	function areAccSame(formAcc, savedAcc)
	{
		for (var i = 0; i < formAcc.infoPairs.length; i++)
		{
			var inf = formAcc.infoPairs[i];
			if (inf.type == KEYTYPE_URL)
				continue;	//ignore urls
			if (!accHasInfo(savedAcc, inf))
				return false;
		}
		return true;	//accounts are same
	}
	
	function accHasInfo(savedAcc, info)
	{
		for (var i = 0; i < savedAcc.infoPairs.length; i++)
		{
			var savedInfo = savedAcc.infoPairs[i];
			if (savedInfo.key == info.key && savedInfo.value == info.value && savedInfo.type == info.type)	//do not check type?
				return true;
		}
		return false;	//savedAcc does not have an info-key pair same as info
	}

	function showLocked()
	{
		unLockedStateCache = false;
		if (state == LOCKED_STATE || state == DISABLED_STATE || state == UNCONFIGURED_STATE)
			return;
		matchingAccounts = [];
		$('#jakilid-button').attr('type', 'button');
		$('#jakilid-button').attr('tooltiptext', tr('Click to unlock Jakilid', 'addon'));
		$('#jakilid-button').addClass('locked').removeClass('unlocked').removeClass('fillAvailable');

		state = LOCKED_STATE;
	}

	function showDisabled()
	{
		unLockedStateCache = false;
		if (state == DISABLED_STATE)
			return;
		matchingAccounts = [];
		$('#jakilid-button').attr('type', 'button');
		$('#jakilid-button').attr('tooltiptext', tr('Jakilid is not running'));
		$('#jakilid-button').addClass('disabled').removeClass('unlocked').removeClass('fillAvailable').removeClass('locked');

		state = DISABLED_STATE;
	}

	function showUnconfigured()
	{
		unLockedStateCache = false;
		if (state == UNCONFIGURED_STATE)
			return;
		$('#jakilid-button').attr('type', 'button');
		$('#jakilid-button').attr('tooltiptext', tr("Jakilid is not configured", 'wizard'));
		$('#jakilid-button').addClass('unconfigured').removeClass('unlocked').removeClass('fillAvailable').removeClass('locked');

		state = UNCONFIGURED_STATE;
	}
	
	function showUnlocked()
	{
		if (state == UNLOCKED_STATE || state == DISABLED_STATE)
			return;
		matchingAccounts = [];
		$('#jakilid-button').attr('type', 'menu-button');
		$('#jakilid-button').attr('tooltiptext',tr('Click to open Jakilid in new tab', 'addon'));
		$('#jakilid-button').addClass('unlocked').removeClass('locked').removeClass('fillAvailable').removeClass('unconfigured');
		$('#openVaultItm').addClass('defaultItem');
		$('#jakilid-button .autoFillMenu').remove();
		$('#accountSeperator').remove();

		state = UNLOCKED_STATE;
	}

	function showAutoFill()
	{
		if (state == DISABLED_STATE)
			return;

		$('#jakilid-button .autoFillMenu').remove();
		$('#accountSeperator').remove();	//add
		$('#jakilid-button').attr('tooltiptext', tr("Click to auto fill '%1'", 'addon', [matchingAccounts[0].title])/*.arg(matchingAccounts[0].title)*/ );
		
		if (state == FILLAVAILABLE_STATE)
			return;
		$('#jakilid-button').attr('type', 'menu-button');
		$('#jakilid-button').addClass('unlocked').removeClass('locked').addClass('fillAvailable');
		$('#openVaultItm').removeClass('defaultItem');

		state = FILLAVAILABLE_STATE;
	}

	function translateAddon()
	{
		$('#mainMenu').attr('dir',locale.localeDirection() ).css('direction', locale.localeDirection());
		$('#login').attr('dir', locale.localeDirection()).css('direction', locale.localeDirection());
		$('#askSave').attr('dir', locale.localeDirection()).css('direction', locale.localeDirection());
		$('#password').attr('placeholder', tr('Enter Master Password', 'addon'));
		$('#LoginBtn').attr('label', tr('Unlock', 'addon'));
		$('#askSaveTxt').attr('value', tr('Would you like to save this login information?', 'addon'));
		$('#askSaveDesc').attr('value', tr('You would be able to auto fill this form by saving it now.', 'addon'));
		$('#openVaultItm').attr('label', tr('Show Jakilid', 'addon'));
		$('#lockItm').attr('label', tr('Lock', 'addon'));
		$('#jakilid-button').attr('label', tr('Jakilid'));
		$('#jakilid-button').attr('tooltiptext', tr('Click to unlock Jakilid', 'addon'));
		$('#SaveLoginBtn').attr('label', tr('Save', 'addon'));
		$('#LoginDetailsBtn').attr('label', tr('Details', 'addon'));
		$('#DontSaveLoginBtn').attr('label', tr('Not now', 'addon'));
	}





	/**
	 * Installs the toolbar button with the given ID into the given
	 * toolbar, if it is not already present in the document.
	 *
	 * @param {string} toolbarId The ID of the toolbar to install to.
	 * @param {string} id The ID of the button to install.
	 * @param {string} afterId The ID of the element to insert after. @optional
	 */
	function installButton(toolbarId, id, afterId)
	{
		if (!document.getElementById(id)) {
			var toolbar = document.getElementById(toolbarId);

			// If no afterId is given, then append the item to the toolbar
			var before = null;
			if (afterId) {
				let elem = document.getElementById(afterId);
				if (elem && elem.parentNode == toolbar)
					before = elem.nextElementSibling;
			}

			toolbar.insertItem(id, before);
			toolbar.setAttribute("currentset", toolbar.currentSet);
			document.persist(toolbar.id, "currentset");

			if (toolbarId == "addon-bar")
				toolbar.collapsed = false;
		}
	}
	function firstRun(extensions) 
	{
		var extension = extensions.get("Jakilid@Jakilid.com");
	 
		if (extension.firstRun)
		{
			installButton("nav-bar", "jakilid-button");
			installButton("addon-bar", "jakilid-button");
		}
	}




	function cacheSessionState()
	{
		JakilidWrapper.isUnlocked(function(nret){
			unLockedStateCache = nret;
			if (nret)	//if session is unLocked call unlock event
				JakilidWrapper.onUnlock();	// == inspectForSuggestion();
		});
		JakilidWrapper.getAvailableProfiles(function(profilesArray) {
			if (!profilesArray || profilesArray.length == 0)
				showUnconfigured();	//Jakilid has not been configured yet, disable addon
		});
	}

	//var localS = "hello <b>salam</b><div style=\"font-size:22px;\">123</div><input type=\"text\"/>";
	//setupPanel(localS);
	//document.getElementById('htmlPanel').openPopup(null, 'overlap', 0, 0);
	function setupPanel(contents, width, height)
	{
		var iframe = document.getElementById("htmlContainer");
		iframe.setAttribute("src","data:text/html;charset=utf-8," + escape(contents));
		iframe.width = width || 300;
		iframe.height = height || 300;
	}


	/******************** main ******************/
	
	//notify whenever tab or current url changes
	// implements nsIWebProgressListener
	var listener = {
		//fires whenever html of current page is ready (state==complete || state==interactive)
		onHtmlReady: null,
		// fires on tab change; update status here;
		onLocationChange: function(aProgress, aRequest, aURI)	{},
		onStateChange: function(aWebProgress, aRequest, aFlag, aStatus)
		{
			//if web page state is ready
			if ((aFlag & Components.interfaces.nsIWebProgressListener.STATE_STOP) && (aFlag & Components.interfaces.nsIWebProgressListener.STATE_IS_WINDOW))
			{
				/*
				var els = content.document.forms;
				for (var i = 0; i < els.length; i++)
				{
				var el = els[i];
				var orgSubmit = el.onsubmit;

				el.onsubmit = function()
				{
					if (typeof orgSubmit !== 'function')
					{
						alert('captured2');
						onFormSubmit({target:this});
						return this.submit();
					}
					var nret = orgSubmit.call(this);
					console.log('submit', nret);
					if (nret)
					{
						alert('captured');
						//onFormSubmit({target:this});
					}
					return nret;
				}
			
				}
				*/
				this.onHtmlReady(true); 
			}
		},
		onProgressChange: function(a, b, c, d, e, f) {},
		onStatusChange: function(a, b, c, d) {},
		onSecurityChange: function(a, b, c) {}
	}

	listener.onLocationChange = onPageChanged;
	listener.onHtmlReady = onPageChanged;

	gBrowser.addProgressListener(
		listener,
		Components.interfaces.nsIWebProgress.NOTIFY_LOCATION
	);


	var idleService = Components.classes["@mozilla.org/widget/idleservice;1"].getService(Components.interfaces.nsIIdleService);


	window.addEventListener("load", function load(event)
	{
		/*********** addon loaded ****************/
		window.removeEventListener("load", load, false); //remove listener, no longer needed
		
		//BUG: if there's any open tab with ui url (for example from previous borwser session), this addon's session would be different from that ui.
		//FIX: look for any open tab with ui url and use its session id
		setupJakilidClient('chrome://Jakilid/content/ui/js/', showDisabled);	//prepare jakilid client, on failure (jakilid not installed) show disabled state

		//initialization
		locale = new i18n( {localePrefix:'chrome://Jakilid/content/ui/locale/',
						"locale": JakilidUI.preferredLanguage(),
						callBack: translateAddon
						} );
		
		//setup event handlers
		$('#jakilid-button').on('command', onButtonClick);
		$('#openVaultItm').on('command', openJakilid);
		$('#lockItm').on('command', lock);
		$('#login').on('popupshown', function() { document.getElementById('password').focus(); } );
		$('#login').on('popuphidde', function() { document.getElementById('password').value=''; } );
		$('#password').on('keypress', function(e) {if (e.keyCode == KeyEvent.DOM_VK_RETURN) auth(); } );
		$('#LoginBtn').on('command', auth);
		$('#DontSaveLoginBtn').on('command', function(e) { e.stopPropagation(); clearLoginSave(); document.getElementById('askSave').hidePopup(); } );
		
		//install toolbar button
		if (Application.extensions)
			firstRun(Application.extensions);
		else
			Application.getExtensions(firstRun);


		//check for session state
		if (JakilidWrapper.isReady())
			cacheSessionState();
		else
			JakilidWrapper.onReady = cacheSessionState;

		setInterval(function()
		{
			if (idleService.idleTime >= idleTimeout*1000)
				JakilidWrapper.lock();
			else
				JakilidWrapper.ping();	//prevent session lock out
		}, 60000);


		gBrowser.addEventListener("DOMContentLoaded", function load(event) {
			if (event.originalTarget == content.document)	//if event is for current tab
				onPageChanged();
				
	        var doc = event.originalTarget; // doc is document that triggered the event
			var win = doc.defaultView; // win is the window for the doc
				
			win.wrappedJSObject.HTMLFormElement.prototype.submit = function(w) {
				return function(s) {
					onFormSubmit({target:this});
					w.call(this, s );
				};
			}(win.HTMLFormElement.prototype.submit);
			
			/*
			var els = doc.forms;
			for (var i = 0; i < els.length; i++)
			{
				var el = els[i];
				
				el.onsubmit = function(w) {
					return function(s) {
						alert("special dom2");
						onFormSubmit({target:this});
						w.call(this, "(" + s + ")");
					};
				}(el.onsubmit);
			}
			*/
		});
		// Capture user submitted forms:
		gBrowser.addEventListener("submit", onFormSubmit, true);	//capture all form submission events

		
	},false);

	//return public methods
	return {
		autoFill:autoFill	//used for autofill menu items
	};
	
}();