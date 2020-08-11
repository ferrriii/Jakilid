/* Copyright Jakilid.com - All Rights Reserved */
//TODO: wrap these contents into an object
var	KEYTYPE_TEXT		= 0;
var	KEYTYPE_PASSWORD	= 1;
var	KEYTYPE_COMMENT		= 2;
var	KEYTYPE_EMAIL		= 3;
var	KEYTYPE_URL			= 4;
var	KEYTYPE_NUMBER		= 5;
var	KEYTYPE_TELL		= 6;
var	KEYTYPE_TAGS		= 7;

/******** i18n related ************/

/*!
 * function overloads:
 * tr(String str)
 * tr(String str, String context)
 * tr(String str, String context, Array args)
 * tr(String str, Array args)
*/
function tr(str, context, args)
{
	var nret;
	//translate
	if (i18n._singletonInstance)
		nret = i18n._singletonInstance.tr(str, (typeof context === 'string' ? context : undefined ) );
	else
		nret = str;	//fallback to original str
	
	if (typeof context === 'object' || typeof args === 'object')
	{
		//replace arguments
		nret = arg(nret, (typeof args === 'object' ? args: context) );
	}
	
	return nret;
}

function arg(str, args)
{
	for(var i=0; i<args.length; i++) {
		str = str.replace(RegExp("\%" + (i+1), 'g'), args[i]);
	}
	return str;
}


//jQuery tr plugin
(function( $ ) {
	$.fn.tr = function(str, context, args) {
		this.text( tr( (typeof str !== 'undefined' ? str : this.text() ), context, args ) );
	};
}( jQuery ));



/*!
	Qt::String::arg function for javascript String object
	/code
		'this %1'.arg(5);	//returns 'this 5'
		'this is %1 and %2 and %1'.arg('1st', '2nd');	//returns 'this is 1st and 2nd and 1st';
	/endcode
*/
/*
String.prototype.arg= function ()
{
	var ret = this;
	for(var i=0; i<arguments.length; i++) {
		ret = ret.replace(RegExp("\%" + (i+1), 'g'), arguments[i]);
	}
	return ret;
};
*/

/**************/

function newInfo(key, value, type)	//used in accountView, ui, formFill
{
	return	{
				'id':		-1,
				'key':		key,
				'value':	value,
				'type':		type
				//order:	
			};
}

function suggestAccTitle(userName, url)	//used in formFill, accountView
{
	var title;
	//url = getLocation(url).hostname;
	if (userName && url)
		title = tr('%1 at %2', 'account', [userName, url]); // .arg(userName, url);
	else if (userName)
		title= userName
	else if (url)
		title = url;
	
	return title;
}

function htmlEncode(strIn)	//used in ui, accountView
{
	return $('<div />').text(strIn).html();
}

function infoTypeName(typeId)	//used in accountView
{
	switch(typeId)
	{
	case KEYTYPE_TEXT:	case '0':	//text
	  return 'text';
	case KEYTYPE_PASSWORD: case '1':		//password
		return 'text';	//NOTE: password is the correct one but displaying as text lets user to see the password
	case KEYTYPE_COMMENT:	case '2':	//comment
		return 'textarea';
	case KEYTYPE_EMAIL:	case '3':	//email
		return 'email';
	case KEYTYPE_URL:	case '4':	//url
		return 'url';
	case KEYTYPE_NUMBER: case '5':	//number
		return 'number';
	case KEYTYPE_TELL: case '6':	//tell
		return 'tell';
	case KEYTYPE_TAGS:	case '7':	//tags
		return 'select2';
	default:
	  return 'text';
	}
}

function infoTypeId(typeName)	//used in addon formFill
{
	if (typeof typeName !== 'string')
		return KEYTYPE_TEXT;
		
	switch(typeName.toLowerCase())
	{
	case 'text':
		return KEYTYPE_TEXT;
	case 'password':
		return KEYTYPE_PASSWORD;
	case 'textarea':
		return KEYTYPE_COMMENT;
	case 'email':
		return KEYTYPE_EMAIL;
	case 'url':
		return KEYTYPE_URL;
	case 'number':
		return KEYTYPE_NUMBER;
	case 'tell':
		return KEYTYPE_TELL;
	default:
	  return KEYTYPE_TEXT;
	}
}