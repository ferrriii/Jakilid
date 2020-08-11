/* Copyright Jakilid.com - All Rights Reserved */
var JakilidWrapper = {isReady: function(){return false;}, prepared: 0};
let JakilidUiWaiter= Promise.resolve()
const isBrowser = () => {
  return navigator.userAgent.indexOf('QtWebEngine') <= 0
}

(function() {

	function createChannel () {
	  return new Promise((resolve, reject) => {
		new QWebChannel(qt.webChannelTransport, function (channel) {
          window.JakilidUI = channel.objects.JakilidUI
		  resolve()
		})
	  })
	}

	if (!isBrowser()) {
		JakilidUiWaiter = createChannel()
	}
	if (isBrowser())	//if JakilidUI has not been already defined (using embedded Jakilid UI)
	{
		window.JakilidUI = {
			browser : function()
			{
				return 'user';
			},
			setPreferredLanguage: function(lang)
			{
				try
				{
					//Normal view
					if (typeof localStorage === 'object')
						localStorage.setItem("language", lang);
					//WARNING: if localStorage doesn't exist nothing would be saved
				} catch(e)
				{
					//Extension view
					var prefManager = Components.classes["@mozilla.org/preferences-service;1"].getService(Components.interfaces.nsIPrefBranch);
					prefManager.setCharPref('extensions.jakilid.language', lang);
				}
			},
			preferredLanguage: function()
			{
				var storedLang;
				
				try
				{
					//Normal view
					if (typeof localStorage === 'object')
						storedLang = localStorage.getItem("language");
				} catch(e)
				{
					//Extension view
					try {
						var prefManager = Components.classes["@mozilla.org/preferences-service;1"].getService(Components.interfaces.nsIPrefBranch);
						storedLang = prefManager.getCharPref('extensions.jakilid.language');
					} catch(e)
					{
						if (prefManager)
							storedLang = prefManager.getCharPref('general.useragent.locale'); /*nothing saved before*/
					}
				}
				
				var userLang = storedLang || navigator.userLanguage;
				return userLang;
			},
			copyToClipboard: function(s)
			{
				const gClipboardHelper = Components.classes["@mozilla.org/widget/clipboardhelper;1"].getService(Components.interfaces.nsIClipboardHelper);
				gClipboardHelper.copyString(s);
			},
			isMaximized: false


		};
	}

})();

function setupJakilidClient(jsPrefix, notInstalled, sid)
{
	var jsPathPrefix = (jsPrefix || 'js/');
	//setup client
	setupEmbedClient(function() {
		setupWebSocketClient(sid, function() {
			setupXhrClient(sid, notInstalled);
		});
	});	
		
	function testClient(failureCallBack)
	{
		setTimeout(function() {
				if (!JakilidWrapper.isReady())
					return failureCallBack();
				var tid = setTimeout(failureCallBack, 3000);
				JakilidWrapper.isUnlocked(function() { clearTimeout(tid); JakilidWrapper.prepared= 1; });
			}, 12000);	//1500ms timeout for websocket connection
	}

	function setupEmbedClient(failureCallBack)
	{
		if (typeof Hendoone === 'undefined')
			return failureCallBack();
		
		addScript(jsPathPrefix + 'jakilid-embed-client.js', function()
		{
			JakilidWrapper = new JakilidEmbedClient();
			testClient(failureCallBack);
		}, failureCallBack);
	}

	function setupWebSocketClient(sid, failureCallBack)
	{
		if (!'WebSocket' in window)
			return failureCallBack();

		//JakilidWebSocketClient
		addScript(jsPathPrefix + 'jakilid-websocket-client.js', function()
		{
			var onLock = JakilidWrapper.onLock;
			var onUnlock = JakilidWrapper.onUnlock;
			var onFailure = JakilidWrapper.onFailure;
			var onReady = JakilidWrapper.onReady;
			var onError = JakilidWrapper.onError;
			JakilidWrapper = new JakilidWebSocketClient(sid);
			//respect old attached event handlers
			JakilidWrapper.onLock = onLock;
			JakilidWrapper.onUnlock = onUnlock;
			JakilidWrapper.onFailure =onFailure;
			JakilidWrapper.onReady = onReady;
			JakilidWrapper.onError = onError;
			testClient(failureCallBack);	
		}, failureCallBack);
	}

	function setupXhrClient(sid, failureCallBack)
	{
		addScript(jsPathPrefix + 'jakilid-jsonp-client.js', function()
		{
			var onLock = JakilidWrapper.onLock;
			var onUnlock = JakilidWrapper.onUnlock;
			var onFailure = JakilidWrapper.onFailure;
			var onReady = JakilidWrapper.onReady;
			var onError = JakilidWrapper.onError;
			JakilidWrapper = new JakilidJsonpClient(sid);
			//respect old attached event handlers
			JakilidWrapper.onLock = onLock;
			JakilidWrapper.onUnlock = onUnlock;
			JakilidWrapper.onFailure =onFailure;
			JakilidWrapper.onReady = onReady;
			JakilidWrapper.onError = onError;
			testClient(failureCallBack);
		}, failureCallBack);
	}

	function addScript(url, loadComplete, failure)
	{
		//allow loading local files only. load urls starting with chrome:// (firefox addon path) and reject urls containing // (like http:// ftp:// ...)
		if ( !/^chrome:\/\//igm.test(url) && /\/\//.test(url) )
		{
			//invalid url
			(typeof failure === 'function' ? failure() : null );
			return;
		}
		var lnk = document.createElementNS('http://www.w3.org/1999/xhtml', 'script');// document.createElement('script');
		lnk.src= url;
		lnk.id= Math.floor(Math.random() * 10000);
		lnk.onload = (typeof loadComplete === 'function' ? loadComplete : null );
		lnk.onerror = (typeof failure === 'function' ? failure : null );
		(document.head||document.documentElement).appendChild(lnk);
	}

};