/* i18n.js version 1.0
 * Copyright 2014 Farshad Shahbazi
 * Released under the MIT license
 * https://github.com/Jakilid/i18n.js
*/
function i18n(options)
{
	if ( i18n._singletonInstance )
		return i18n._singletonInstance;
	i18n._singletonInstance = this;
	
	this.settings = $.extend({
		baseLocale: "en-US",
		locale: "en-US",
		localePrefix: 'locale/',
		callBack: null
	}, options );	
	
	this.lastStyleId;	//last translations style id
	this.lastLangScrId;
	this.currentLang = this.settings.baseLocale;
	
	var html = $("html");
	if (html.length !== 0)
		html.attr('data-dir', html.attr('dir') ? html.attr('dir') : 'ltr' );
	if (!this.changeTranslation( this.settings.locale ))
		setTimeout(this.translateCompleted, 0);
}

i18n.prototype.tr = function(str, context){
	
	if (typeof i18n.lang !=='object')
		return str;	//when there's no language object fallback to original str
	if (i18n.lang.langCode.toLowerCase() == 'en')
		return str;	//bypass translation if current language is English (because str would be same as translated one)
	if (typeof context === 'undefined')
		var s = i18n.lang.context[str];
	else if (typeof i18n.lang.context[context] !== 'undefined')
		var s = i18n.lang.context[context][str];
	
	var ret;
	if (typeof s === 'undefined')
		ret = str.toString();	//fallback to original str when there's no translation
	else
		ret = s.toString();
	
	return ret;
}
i18n.prototype.toLocalNumber = function(n)
{
	if (typeof i18n.lang !=='object' || typeof i18n.lang.toLocalNumber !=='function')
		return n;//when there's no language object fallback to original number
	
	return i18n.lang.toLocalNumber(n);
}
i18n.prototype.toLocalDateStr = function(d)
{
	if (typeof i18n.lang !=='object' || typeof i18n.lang.toLocalDateStr !=='function')
		return d.getFullYear() + '-' + (d.getMonth()+1) + '-' + d.getDate();//when there's no language object fallback to ISO date format: YYYY-MM-DD
	
	return i18n.lang.toLocalDateStr(d);
}
i18n.prototype.currentLanguage = function()
{
	return this.currentLang;
}
i18n.prototype.localeDirection = function()
{
	try
	{
		return i18n.lang.direction.toLowerCase();
	}
	catch(e)
	{
		return 'ltr';
	}
}

i18n.prototype.changeTranslation = function(languageCode)
{
	if (this.currentLang == languageCode)// ||  (typeof i18n.lang !== 'undefined' && (i18n.lang.langCode == languageCode)))
		return false;	
	
	i18n.exec('unload');
	if (languageCode == this.settings.baseLocale)
	{
		//do not load language script for default language, just reset.
		i18n.clearLastStyle();//delete prevoius style (if any)
		i18n.clearLastLangScr();//delete prevoius lang script (if any)
		delete i18n.lang;//clear i18n.lang
		$("html").attr('dir', $("html").attr('data-dir') );
		this.currentLang = languageCode;
		this.translateCompleted();
		return true;
	}
	
	i18n.addScript(this.settings.localePrefix + languageCode + '/' + languageCode + '.js', function(){
		i18n.exec('load');
		i18n.clearLastLangScr( $(this).attr('id'));
		var html = $("html");
		if (html.length !== 0)
			html.attr('dir', i18n.lang.direction);
		if (typeof i18n.lang.style !== 'undefined' && html.length !== 0)
		{
			i18n.addStyle(i18n._singletonInstance.settings.localePrefix + languageCode + '/' + i18n.lang.style, function()
			{
				i18n.clearLastStyle($(this).attr('id'));
				i18n._singletonInstance.translateCompleted();
			} );
		}
		else
		{
			i18n.clearLastStyle();
			i18n._singletonInstance.translateCompleted();
		}
	} );
	
	this.currentLang = languageCode;
	return true;
}

i18n.prototype.translateCompleted = function()
{
	if (typeof i18n._singletonInstance.settings.callBack === 'function')
		i18n._singletonInstance.settings.callBack();
}

i18n.exec = function(func)
{
	if (typeof i18n.lang !== 'object')
		return false;

	if (typeof i18n.lang[func] === 'function')
		return i18n.lang[func]();
}

i18n.clearLastStyle = function(newId)
{
	if (i18n._singletonInstance.lastStyleId)
		$('#' + i18n._singletonInstance.lastStyleId).remove();
	i18n._singletonInstance.lastStyleId = newId;
}

i18n.clearLastLangScr = function(newId)
{
	if (i18n._singletonInstance.lastLangScrId)
		$('#' + i18n._singletonInstance.lastLangScrId).remove();
	i18n._singletonInstance.lastLangScrId = newId;
}

i18n.addStyle = function(url, loadComplete)
{
	//allow loading local files only. load urls starting with chrome:// (firefox addon path) and reject urls containing // (like http:// ftp:// ...)
	if ( !/^chrome:\/\//igm.test(url) && /\/\//.test(url) )
	{
		//invalid url
		//(typeof failure === 'function' ? failure() : null );
		return;
	}
	var lnk=document.createElement('link');
	lnk.href= url;
	lnk.rel='stylesheet';
	lnk.id= Math.floor(Math.random() * 10000);
	lnk.type='text/css';
	lnk.onload = (typeof loadComplete === 'function' ? loadComplete : null );
	(document.head||document.documentElement).appendChild(lnk);
}


/*!
	loads and executes script at the given url
*/
i18n.addScript = function(url, loadComplete)
{
	//allow loading local files only. load urls starting with chrome:// (firefox addon path) and reject urls containing // (like http:// ftp:// ...)
	if ( !/^chrome:\/\//igm.test(url) && /\/\//.test(url) )
	{
		//invalid url
		//(typeof failure === 'function' ? failure() : null );
		return;
	}
	var lnk = document.createElementNS('http://www.w3.org/1999/xhtml', 'script');// document.createElement('script');
	lnk.src= url;
	lnk.id= Math.floor(Math.random() * 10000);
	lnk.onload = (typeof loadComplete === 'function' ? loadComplete : null );
	(document.head||document.documentElement).appendChild(lnk);
}



