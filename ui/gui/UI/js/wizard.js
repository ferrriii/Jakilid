var wizard = function(){

	var startUi = null;
	var tmpProfile = {};
	tmpProfile.name = 'default';

	function translateWizard()
	{
		//translate wizard (first page)
		$('#wizard').attr('title', tr('Jakilid Configuration Wizard', 'wizard'));
		try
		{
			$('#wizard').dialog('option', 'title', tr('Jakilid Configuration Wizard', 'wizard'));
			$('#wizard').dialog('option', 'buttons', [{text: tr("Next", 'wizard'), click: welcomeNextClick}] );
		}catch(e) {	/* dialog has not been created yet */ }
		$('#welcomeWizard').tr('Welcome to Jakilid Password Manager', 'wizard');
		$('#textWizard').tr('There are a few things to configure, this will guide you to do this.', 'wizard');
		$('#languageLblWizard').tr('Choose your language:', 'wizard');
		$('#advancedLblWizard').tr('Proceed advanced configurations.', 'wizard');
	}

	function startWizard(callBack)
	{
		startUi = callBack;
		
		$('<div></div>', {id:"wizard", title: tr('Jakilid Configuration Wizard', 'wizard') })
		.appendTo('body')
		.dialog({
			autoOpen: false,
			width: 470,
			height:470,
			modal:true,
			buttons: [{text: tr("Next", 'wizard'), click: setupStep1}],
			show: {
				effect: "clip",
				duration: 750
			},
			hide: {
				effect: "fade",
				duration: 300
			},
			closeOnEscape: false,
			open: function(event, ui) { $(".ui-dialog-titlebar-close", ui.dialog).hide(); },
			close: function (event, ui) {
				$(this).remove();
			}
		});

		setupStep0();
		$( "#wizard" ).tabs().dialog( "open" );
	}

	function setupStep0()
	{
		$('#wizard').html('<h2 id="welcomeWizard">' + tr('Welcome to Jakilid Password Manager', 'wizard') + '</h2>' + 
						'<p id="textWizard">' + tr('There are a few things to configure, this will guide you to do this.', 'wizard') + '</p>' +
						'<form class="pure-form pure-form-stacked">' +
						'<fieldset>' + 
						'<legend></legend>' + 
						'<label for="lang" id="languageLblWizard">' + tr('Choose your language:', 'wizard') + '</label><select id="lang"><option value="en-US" dir="auto">English</option><option value="fa-IR" dir="auto">فارسی</option></select>' + 
						'<label for="advanced"><input id="advanced" type="checkbox"> <span id="advancedLblWizard">' + tr('Proceed advanced configurations.', 'wizard') + '</span></label>' + 
						'</fieldset>' + 
						'</form>');
		$('#lang').val(locale.currentLanguage()).change(function() {
			locale.changeTranslation($(this).val());
		});
		
		$( "#wizard" ).dialog('option', 'buttons', [{text: tr("Next", 'wizard'), click: welcomeNextClick }]);	
	}

	function welcomeNextClick()
	{
		if ($('#advanced').is(':checked'))
			setupStep1();
		else
		{
			tmpProfile.db = JakilidUI.preferedDatabasePath;
			setupStep2(1);
		}
	}

	function setupStep1()
	{
		$('#wizard').html('<p>' + tr('Jakilid will save your data in your "Documents" directory by default.','wizard') + '</p><p>' + tr('If you wish, You can change this path or use an old Jakilid file.', 'wizard') + '</p><p>' + tr('If you prefer default settings, just click Next.', 'wizard') + '</p>' + 
		'<div id="databaseCreation">' + 
		'<h4>' + tr('I want change default file path', 'wizard') + '</h4>' + 
		'<div><span>' + tr('Select a new file:', 'wizard') + '</span>' + 
		'<form id="newDbForm" class="pure-form"> \
		<fieldset> \
				<input id="dbPath" type="text" placeholder="' + tr('Type or browse for a file', 'wizard') + '"> \
				<button type="button" class="pure-button pure-button-primary" onClick="JakilidUI.browseSaveFile($(this).val(), file => $(\'#dbPath\').val(file)) ">' + tr('Browse', 'wizard') + '</button> \
		</fieldset> \
	</form>' + 
		'</div>' +
		'<h4>' + tr('I have an old Jakilid file', 'wizard') + '</h4>' + 
		'<div><span>' + tr('Choose your old file:', 'wizard') + '</span>' +
		'<form id="existingDbForm" class="pure-form"> \
		<fieldset> \
				<input id="dbExistingPath" type="text" placeholder="' + tr('Type or browse for an existing Jakilid file', 'wizard') + '"> \
				<button type="button" class="pure-button pure-button-primary" onClick="JakilidUI.browseOpenFile($(this).val(), file => $(\'#dbExistingPath\').val(file)) ;">' + tr('Browse', 'wizard') + '</button> \
		</fieldset> \
	</form>' + 
		'</div>' +
		'</div>' +
		'<div id="errorMessage" class="invisible"><span class="icon-notification" style="color:red"></span> <span id="errorSpan"></span></div>'
		);

		$('#dbPath').val(JakilidUI.preferedDatabasePath);
		$( "#newDbForm,#existingDbForm" ).submit(function( event )
		{
			event.preventDefault();
			processDatabaseStep();
		});	
			
		$( "#databaseCreation" ).accordion( {active: false,
											 collapsible: true});
		$( "#wizard" ).dialog('option', 'buttons', [{text: tr("Previous", 'wizard'), click: setupStep0},
													{text: tr("Next", 'wizard'), click: processDatabaseStep}]);
	}

	async function processDatabaseStep()
	{
		var dbType = $('#databaseCreation').accordion('option', 'active');
		if (dbType === 0)
		{
			//change new database path
			tmpProfile.db = $('#dbPath').val();
			if (await validateDbFile(tmpProfile.db, true))
				setupStep2();	//create new database
			else
				$('#dbPath').focus();
		}
		else if (dbType === 1)
		{
			//old database
			tmpProfile.db = $('#dbExistingPath').val();
			if (await validateDbFile(tmpProfile.db, false))
				setupStep2_existing();	//use existing one
			else
				$('#dbExistingPath').focus();
		}
		else
		{
			//default
			tmpProfile.db = JakilidUI.preferedDatabasePath;
			setupStep2();	//create new database
		}
	}

	function validateDbFile(db, isNew)
	{
		return new Promise((resolve, reject) => {
			if (db == '')
			{
				$('#errorSpan').text(tr('Please enter a file address.','wizard'));
				$('#errorMessage').removeClass('invisible');
				reject(false)
			}
			var err = JakilidUI.isValidDbFile(db, isNew, err => {
				if (err != '')
				{
					$('#errorSpan').text(tr(err,'wizard'));
					$('#errorMessage').removeClass('invisible');
					reject(false);
				}
				resolve(true);
			});			
		})
	}

	function setupStep2(isNotAdvanced)
	{
		$('#wizard').html('<p>' + tr('Please create a master password for accessing all your password.', 'wizard') + '</p><form id="dbPassword" class="pure-form pure-form-stacked"> \
		<fieldset> \
				<input id="Pass" type="Password" placeholder="' + tr('Use at least 8 characters', 'wizard') + '" pattern=".{8,}" required="true" size="25"> \
				<input id="Pass-Match" type="Password" placeholder="' + tr('Type it again', 'wizard') + '" pattern=".{8,}" required="true" size="25"> \
				<input type="submit" style="display:none"/> \
		</fieldset> \
	</form><div id="errorMessage" class="invisible"><span class="icon-notification" style="color:red"></span> <span id="errorSpan"></span></div><p><span class="icon-notification" style="color:red"></span> ' + tr('WARNING: without this password you won\'t be able to access your passwords and you will lost all your data.', 'wizard') + '</p>');	
		
		$( "#wizard" ).dialog('option', 'buttons',
								[{text: tr("Previous", 'wizard'), click: function() {if (isNotAdvanced) setupStep0(); else setupStep1(); }},
								{text: tr("Finish", 'wizard'), click: createNewDb}]

		);

		$('#Pass').focus();
		$( "#dbPassword" ).submit(function( event )
		{
			event.preventDefault();
			createNewDb();
		});	
		
	}

	function createNewDb()
	{
		validatePassword(function(){
			//password passed validations
			tmpProfile.pass = $('#Pass').val();
			
			JakilidWrapper.createProfile('default', tmpProfile.db, locale.currentLanguage(), tmpProfile.pass, true/*new*/, function(nret){
				if (nret)
				{
					$('#wizard').dialog('close');
					startUi();
				}else
				{
					$('#errorSpan').text(tr('An error occured, please check inputs and try again.','wizard'));	
					$('#errorMessage').removeClass('invisible');				
				}
			});
		});	
	}

	function validatePassword(validationPassedFunc)
	{
		if ($('#Pass').val() == '' && $('#Pass-Match').val() == '')
		{
			$('#errorSpan').text(tr('Please specify a password to protect your data.','wizard'));
			$('#errorMessage').removeClass('invisible');
			$('#Pass').focus();
		}
		if ($('#Pass').val() != $('#Pass-Match').val())
		{
			$('#errorSpan').text(tr('Passwords don\'t match. Try again?','wizard'));
			$('#errorMessage').removeClass('invisible');
			$('#Pass-Match').focus();
		}
		
		JakilidWrapper.isPassStrong($('#Pass').val(), function(err){
			if (err != '')
			{
				$('#errorSpan').text(tr(err,'wizard'));
				$('#errorMessage').removeClass('invisible');
				$('#Pass').focus();		
			}else
				validationPassedFunc();
		});
	}

	function setupStep2_existing()
	{
		$('#wizard').html('<p>' + tr('Please enter the master password for this database to access all it\'s contents.', 'wizard') + '</p><form id="dbPassword" class="pure-form pure-form-stacked"> \
		<fieldset> \
				<input id="Pass" type="Password" placeholder="' + tr('Enter Master Password', 'wizard') + '"> \
		</fieldset> \
	</form><div id="errorMessage" class="invisible"><span class="icon-notification" style="color:red"></span> <span id="errorSpan"></span></div>');

		$('#Pass').focus();
		$( "#dbPassword" ).submit(function( event )
		{
			event.preventDefault();
			createExistingDbProfile();
		});	

		$( "#wizard" ).dialog('option', 'buttons', [{text: tr("Previous", 'wizard'), click: setupStep1},
													{text: tr("Finish", 'wizard'), click: createExistingDbProfile}]
							);
	}

	function createExistingDbProfile()
	{
		if ($('#Pass').val() == '')
		{
			$('#errorSpan').text(tr('Please enter master password of the selected database.','wizard'));
			$('#errorMessage').removeClass('invisible');
			$('#Pass').focus();
			return;
		}
		tmpProfile.pass = $('#Pass').val();
		
		JakilidWrapper.createProfile('default', tmpProfile.db, locale.currentLanguage(), tmpProfile.pass, false /*is not new*/, function(nret){
			if (!nret)
			{
				$('#errorSpan').text(tr('Password is wrong or the selected database file is invalid!','wizard'));
				$('#errorMessage').removeClass('invisible');			
				$('#Pass').focus();
				return;
			}
			$('#wizard').dialog('close');
			startUi();	
		});
	}

	return {
		startWizard:startWizard,
		translateWizard:translateWizard
	};
}();