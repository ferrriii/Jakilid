/* popup.js version 1.0
 * Copyright 2015 Farshad Shahbazi
 * Released under the MIT license
 * https://github.com/Jakilid/popup.js
*/
(function ( $ ) {
	$.fn.popup = function(anchorEl, options ) {

		var close = function(popupId)
		{
			var data = $(this).data('popupPlugin');
			if (!data)
				data = $("#" + popupId).data('popupPlugin');
			$(this).removeData('popupPlugin').unbind("closePopup");
			var popupEl = $("#" + data.popupId );
			popupEl[data.settings.hideEffect](data.settings.hideOption, function(){ $(this).remove();});
			$(document).off("mouseup.hidepopup." + popupEl.attr('id'));
		}

		if (anchorEl == 'close')
		{
			close($(this).data('popupPlugin').popupId);
			return;
		}

		var settings = $.extend({
			// These are the defaults.
			hideEffect: 'fadeOut',
			hideOption: 'fast',
			width: null,
			height: null,
			style: 'background-color:white; box-shadow: 0px 0px 6px #ccc; border:1px solid #ddd;'
		}, options );

		var contentEl= this;

		var anchorPos = anchorEl.offset();
		var anchorW = anchorEl.outerWidth();
		var anchorH = anchorEl.outerHeight();
		var popupId = 'popup' + Math.floor(Math.random() * 1000);

		var menuEl = $('<div></div>', {id: popupId, style: settings.style})
		.css('position', 'absolute')
		.css('z-index', 1001)
		.append(contentEl)
		.hide()
		.appendTo('body');

		if (settings.width != null)
			menuEl.css('width', settings.width);
		if (settings.height)
			menuEl.css('height', settings.width);

		var meW = menuEl.outerWidth();

		//set popup position
		menuEl.css('left', anchorPos.left + ((anchorW - meW)/2) )
		.css('top', anchorPos.top + anchorH)

		$(document).on("mouseup.hidepopup." + popupId , function (e)
		{
			var popupEl = $("#" + popupId);

			if (!popupEl.is(e.target) // if the target of the click isn't the popupEl...
				&& popupEl.has(e.target).length === 0) // ... nor a descendant of the popupEl
			{
				close(popupId);
			}
		});	

		menuEl.data('popupPlugin', {'settings': settings, 'popupId': popupId} );
		
		//TODO:remove this, instead add static method to close all popups
		menuEl.bind("closePopup",function(){
    		close(popupId);
   		});

		return menuEl;
	};
}( jQuery ));
