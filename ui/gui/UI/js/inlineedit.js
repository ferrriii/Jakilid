/* InLineEdit.js version 1.0
 * Copyright 2014 Farshad Shahbazi
 * Released under the MIT license
 * https://github.com/Jakilid/InLineEdit.js
*/
(function( $ ) {
	$.fn.inLineEdit = function( options ) {
		if (typeof options === 'undefined' || options == 'create')
			this.each(function() {	create($(this)); })
		else if (options == 'destroy')
			this.each(function() {	destroy($(this)); })
		else if (options == 'value')
			return value(this);	//TODO: use foreach and return collection if this is collection
		else if (options == 'input')
		{
			var r= [];
			this.each(function() {	r.push( input($(this)) ); });
			return $(r).map (function () {return this.toArray(); } );
		}	
		return this;
	};
	
	var create = function(itm)
	{
		var val = itm.text(),
		title =itm.attr('data-title'),
		minW =itm.attr('data-minwidth'),
		maxW =itm.attr('data-maxwidth'),
		required =itm.attr('data-required'),
		emptyWidth = 0,
		emptyHeight = 0,
		typ =itm.attr('data-type'),
		inlineId, id, inp;
		
		if (!typ)
			typ = 'text';	//default input type

		if (!minW)
			minW = 15;
		if (!maxW)
			maxW = 170;
		if (title)
		{
			emptyWidth = d(itm.text(title));//width of title
			emptyHeight = d(itm, 1);//height of title
			itm.text(val);	//restore item's text
		}
		else
			emptyWidth = minW;

		inlineId = itm.attr('data-inlineid');
		if (inlineId)	//update already created inline edit
		{
			if (required)
				$('#' + inlineId).attr('required', '1');
			$('#' + inlineId)
			.attr('type', typ)
			.val(val)
			.trigger('input');	//fire input(change) event to set apropriate width
			//TODO: update input event handler with new properties (maxW, emptyWidth,...)
			return;
		}

		id = Math.floor(Math.random() * 10000);
		
		inp = (typ == 'textarea' ? $('<textarea></textarea>') : $('<input>'));
		inp.on('focus', function(e){
			if (!$(this).is(':focus'))
				$(this).select();
		})
		.val(val)
		.attr('id', id)
		.attr('type', typ)
		.attr('placeholder', title)
		.css('max-width', maxW + 'px')
		.css('min-width', minW + 'px')
		.width(val == '' ? emptyWidth : d(itm) )
		.addClass('inlineInput');
		if (typ == 'textarea')
			inp.height(val == '' ? emptyHeight : d(itm,1)).css('resize', 'none').css('overflow','hidden');
		if (required)
			inp.attr('required', '1');
		var styles = ['font','font-famiy','font-size','font-weight','color','padding','padding-top','padding-right','padding-bottom','padding-left','margin','margin-top','margin-right','margin-bottom','margin-left','float','position','display'];
		var cssLen = styles.length;
		for (var i = 0; i < cssLen; i++) {
			inp.css(styles[i], itm.css(styles[i]));	//copy css from item to input
		}
		
		inp.insertAfter(itm)
		.on('input', function() {
			itm.text($(this).val());
			$(this).width($(this).val() == '' ? emptyWidth : d(itm) );	//set width according to text changes
			if (typ == 'textarea')
				$(this).height($(this).val() == '' ? emptyHeight : d(itm,1) );
		});

		itm.attr('data-orgdisplay', itm.css('display'))
		.css('display', 'none')
		.attr('data-inlineid', id);
		inp = null;
	}
	
	var value = function(itm)
	{
		var inlineId = itm.attr('data-inlineid');
		if (inlineId)
			return $('#' + inlineId).val();
		else
			return itm.text();
	}
	
	var destroy = function(itm)
	{
		var inlineId = itm.attr('data-inlineid');
		if (!inlineId)
			return;

		itm.text($('#' + inlineId).val());
		$('#' + inlineId).remove();
		itm.css('display', itm.attr('data-orgdisplay'))
		.removeAttr('data-inlineid')
		.removeAttr('data-orgdisplay');
	}
	
	var input = function(itm)
	{
		var inlineId = itm.attr('data-inlineid');
		if (!inlineId)
			return;

		return $('#' + inlineId);
	}
	
	var d = function(itm, h)
	{
		var isH = (h == 1),
		l = (isH ? itm.height() : itm.width()),
		str = itm.text(),
		extra = 0,
		t;
		
		extra = str.length - str.trim().length;	//WARNING: trim is not is defined in IE<9
		t =  (l * extra) / (isH ? str.split(/\n/g).length : str.length);	//calculate extra width/height for untrimmed string
		if (t > 0 && isFinite(t) && !isNaN(t) && (!isH || /\n$/g.test(str) /*FIX: when str had a space at last height was doubled!*/) )
			l += t+5;//5 extra margin
		
		return l+1;
	}
}( jQuery ));