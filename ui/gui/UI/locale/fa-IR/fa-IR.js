/* Copyright Jakilid.com - All Rights Reserved */
i18n.lang = {
	"langCode": "fa-IR",
	"langName": "فارسی - ایران",
	"fromCode": "en-US",
	"direction": "rtl",
	"style": "fa-IR.css",
	toLocalNumber: function(n) { return n.toString().replace(/\d/gi, function(t) {return String.fromCharCode(parseInt(t) + 1776);} );},
	toLocalDateStr: function(d) { var jdate =this.gregorian_to_jalali(d.getFullYear(), d.getMonth()+1, d.getDate()); return this.toLocalNumber(jdate.year + "/" + jdate.month + "/" + jdate.day); },
	//suggestInputName: function(input, index, inputsArray) {},
	"load": function () {$('#searchBtn').addClass('icon-search-rtl').removeClass('icon-search');},
	"unload": function () {$('#searchBtn').addClass('icon-search').removeClass('icon-search-rtl');},
	"context": {
		"Jakilid Password Manager": "مدیریت کلمات عبور جاکلید",
		"Jakilid": "جاکلید",
		"jakilid.com": "سایت جاکلید",
		"licensing information": "مجوزهای استفاده",
		"feedback": "ارسال نظر",
		"Copyright © 2014 by jakilid.com all rights reserved.": "تمام حقوق این برنامه متعلق به jakilid.com است.",
		"Build Date: %1": "تاریخ ساخت: %1",
		"Version %1": "نسخه %1",
		"About Jakilid Password Manager": "درباره مدیریت کلمات عبور جاکلید",
		"About Jakilid": "درباره جاکلید",
		"Lock (Ctrl+L)": "قفل کن (Ctrl+L)",
		"Configurations": "تنظیمات",
		"Minimize": "برو پایین",
		"Maximize": "تمام صفحه",
		"Quit": "خروج",
		"Search...": "جستجو...",
		"Search": "جستجو",
		"Clear results": "پاک کردن اطلاعات",
		"dismiss": "باشه",
		"Add new item (Ctrl+N)": "افزودن اطلاعات جدید (Ctrl+N)",
		"Unlock Jakilid": "باز کردن قفل جاکلید",
		"Password": "کلمه عبور",
		"Created:": "تاریخ ایجاد:",
		"Labels:": "دسته‌بندی:",
		"login": "ورود",
		"OK": "باشه",
		"Cancel": "بیخیال",
		"Retry": "دوباره امتحان کن",
		"Visit Jakilid download page": "صفحه دانلود جاکلید را باز کن",
		"Restore Down": "کوچک کردن",
		"Access Denied": "عدم دسترسی",
		"Edit item": "ویرایش اطلاعات",
		"Delete item": "حذف اطلاعات",
		"That password was wrong. Please try again.": "کلمه عبور وارد شده اشتباه بود. لطفا دوباره امتحان کنید.",
		"No Title": "بدون عنوان",
		/* results str - begin */
		"Nothing matched \"%1\"": "موردی برای \"%1\" پیدا نشد",
		"There's no item to display. Try adding a new one.": "هیچ اطلاعات وجود ندارد. افزودن اطلاعات جدید را امتحان کنید",
		"Displaying %1 out of %2 items containing \"%3\"": "نمایش %1 مورد اطلاعات از %2 مورد اطلاعات حاوی \"%3\"",
		"Displaying %1 items containing \"%2\"": "نمایش %1 مورد اطلاعات حاوی \"%2\"",
		"Displaying the only item containing \"%1\"": "نمایش تنها مورد اطلاعات حاوی \"%1\"",
		"Displaying %1 out of %2 items": "نمایش %1 مورد اطلاعات از مجموع %2 مورد",
		"Displaying %1 items": "نمایش تمامی %1 مورد اطلاعات",
		"Displaying the only item": "نمایش تنها مورد اطلاعات",
		"Nothing matched \"%1\" from \"%2\" category": "موردی برای \"%1\" در دسته \"%2\" پیدا نشد",
		"Displaying %1 out of %2 items containing \"%3\" from \"%4\" category": "نمایش %1 مورد اطلاعات از %2 مورد اطلاعات حاوی \"%3\" در دسته \"%4\"",
		"Displaying %1 items containing \"%2\" from \"%3\" category": "نمایش %1 مورد اطلاعات حاوی \"%2\" در دسته \"%3\"",
		"Displaying the only item containing \"%1\" from \"%2\" category": "نمایش تنها مورد اطلاعات حاوی \"%1\" در دسته \"%2\"",
		"Displaying %1 out of %2 items from \"%3\" category": "نمایش %1 مورد اطلاعات از مجموع %2 مورد در دسته \"%3\"",
		"Displaying %1 items from \"%2\" category": "نمایش تمامی %1 مورد اطلاعات در دسته \"%2\"",
		"Displaying the only item from \"%1\" category": "نمایش تنها مورد اطلاعات در دسته \"%1\"",
		/* results str - end */
		"There was an error while accessing this item. Please try again.": "هنگام دسترسی به این مورد مشکلی پیش آمد. لطفا دوباره تلاش کنید.",
		"Failed to show item": "مشکل در نمایش اطلاعات",
		"Failed to edit item": "مشکل در ویرایش اطلاعات",
		"Yes, Delete it": "بله، حذفش کن",
		"Cancel Delete": "بیخیال حذف",
		"Confirm Delete": "حذف را تایید کنید",
		"You will not be able to restore this item, are you sure?": "این اطلاعات دیگر قابل بازیابی نخواهند بود، مطمئن هستید؟",
		"Failed to delete item": "مشکل در حذف اطلاعات",
		"Data were saved. Click here to view it.": "اطلاعات ذخیره شد. برای مشاهده آن اینجا کلیک کنید.",
		"There was an error while saving data. Please try again.": "هنگام ذخیره اطلاعات مشکلی پیش آمد. لطفا دوباره تلاش کنید.",
		"Failed to save item": "مشکل در ذخیره اطلاعات",
		"Yes, Discard Changes": "آره، بیخیال تغییرات",
		"No, wait!": "نه، وایسا!",
		"Data have been changed": "اطلاعات تغییر کرده است",
		"Are you sure you want to discard changes?": "آیا مطمئن هستید می خواهید تغییرات را دور بریزید؟",
		"Jakilid is not running": "جاکلید در حال اجرا نیست",
		"Please install and run Jakilid.\nVisit http://Jakilid.com for downloading Jakilid Password Manager.": "لطفا جاکلید را نصب و اجرا کنید.\nبرای دانلود مدیریت کلمات عبور جاکلید به http://Jakilid.com مراجعه نمایید.",
		"config": {
			"How many minutes stay inactive before locking up?": "برنامه بعد از چند دقیقه به صورت خودکار قفل شود؟",
			"Change Master Password": "تغییر کلمه‌ی عبور اصلی",
			"General Settings": "تنظیمات اصلی",
			"OK": "باشه",
			"Cancel": "بیخیال",
			"Database Settings": "تنظیمات پایگاه داده",
			"Current Password": "کلمه عبور فعلی",
			"Type current password": "کلمه عبور فعلی را وارد کنید",
			"New Password": "کلمه عبور جدید",
			"Type New Password": "کلمه عبور جدید را وارد کنید",
			"Confirm New Password": "کلمه عبور جدید را تایید کنید",
			"Type New Password Again": "کلمه عبور جدید را دوباره وارد کنید",
			"Database: %1": "پایگاه داده: %1",
			"The value you entered for auto lockout is invalid. Please enter a number between 1 and 999.": "مقداری که برای زمان قفل خودکار وارد کرده اید اشتباه است. لطفا یک عدد بین 1 و 999 وارد کنید.",
			"Wrong timeout value": "مقدار اشتباه",
			"Choose your language:": "زبان خود را انتخاب کنید:",
			"There was an error while saving your settings. Please try again.": "هنگام ذخیره تنظیمات خطایی پیش آمد. لطفا دوباره تلاش کنید.",
			"Failed to save settings": "تنظیمات ذخیره نشد",
			"Please enter your current password": "لطفا کلمه عبور فعلی خود را وارد کنید",
			"New passwords don't match. Try again?": "کلمات عبور جدید یکی نیستند. دوباره امتحان کنید؟",
			"Your master password was successfully changed.": "کلمه عبور شما با موفقیت تغییر داده شد.",
			"Password successfully changed": "کلمه عبور تغییر کرد",
			"There was an error while changing master password. Please try again.": "هنگام تغییر کلمه عبور خطایی پیش آمد. لطفا دوباره تلاش کنید.",
			"Failed change master password": "کلمه عبور تغییر نکرد"
		},
		"upgrade": {
			"Error while upgrading Database": "خطا هنگام به روز رسانی پایگاه داده",
			"Database successfully upgraded to current version": "پایگاه داده با موفقیت به نسخه فعلی به روز رسانی شد",
			"Database Version Mismatch": "عدم تطابق نسخه پایگاه داده",
			"This database is for an older version of Jakilid. We tried to upgrade it to current version but we failed! Please use latest Jakilid version or try upgrading the database again.": "این پایگاه داده برای یک نسخه قدیمی جاکلید است. ما تلاش کردیم تا آن را به نسخه فعلی به روز رسانی کنیم ولی نتوانستیم! لطفا از آخرین نسخه جاکلید استفاده کنید و یا دوباره به روز رسانی را امتحان کنید.",
			"This database is saved using a higher version of Jakilid. Your data are safe but we don't know how they've been saved, Please download and use the latest Jakilid version to access this database properly.": "این پایگاه داده با استفاده از یک نسخه بالاتر جاکلید ذخیره شده است. اطلاعات شما سالم هستند ولی ما نمی‌دانیم چطور ذخیره شده‌اند. لطفا آخرین نسخه جاکلید را دانلود و استفاده کنید تا به درستی به اطلاعات خود دسترسی پیدا کنید."
		},
		"wizard": {
			"Jakilid Configuration Wizard": "راهنمای تنظیمات جاکلید",
			"Next": "ادامه",
			"Welcome to Jakilid Password Manager": "به مدیریت کلمات عبور جاکلید خوش آمدید",
			"There are a few things to configure, this will guide you to do this.": "این صفحه شما را برای انجام تنظیمات جاکلید راهنمایی می کند.",
			"Choose your language:": "زبان خود را انتخاب کنید:",
			"Proceed advanced configurations.": "تنظیمات را به صورت پیشرفته انجام بده.",
			"Jakilid will save your data in your \"Documents\" directory by default.": "جاکلید به صورت پیش فرض اطلاعات شما را در پوشه \"مستندات\" ذخیره می کند.",
			"If you wish, You can change this path or use an old Jakilid file.": "در صورت تمایل می توانید این مسیر را تغییر دهید و یا از یک فایل قدیمی جاکلید استفاده کنید.",
			"If you prefer default settings, just click Next.": "اگر تنظیمات پیش فرض را می پسندید فقط بر روی ادامه کلیک کنید.",
			"I want change default file path": "می خواهم مسیر ذخیره اطلاعات پیش فرض را تغییر دهم",
			"Select a new file:": "یک فایل جدید انتخاب کنید:",
			"Type or browse for a file": "آدرس فایلی را بنویسید یا آن را انتخاب کنید",
			"Browse": "انتخاب",
			"I have an old Jakilid file": "من یک فایل قدیمی جاکلید دارم",
			"Choose your old file:": "فایل قدیمی خود را انتخاب کنید:",
			"Type or browse for an existing Jakilid file": "آدرس فایل قدیمی را بنویسید یا آن را انتخاب کنید",
			"Previous": "قبلی",
			"Please enter a file address.": "لطفا آدرس یک فایل را وارد کنید.",
			"Please create a master password for accessing all your password.": "لطفا کلمه عبور اصلی که با استفاده از آن به سایر اطلاعات دسترسی پیدا خواهید کرد را ایجاد نمایید.",
			"Use at least 8 characters": "حداقل از 8 حرف استاده کنید",
			"Type it again": "کلمه عبور را دوباره بنویسید",
			"WARNING: without this password you won't be able to access your passwords and you will lost all your data.": "هشدار: بدون این کلمه عبور قادر به دستیابی به اطلاعات خود نخواهید بود و در صورت فراموش کردن آن تمامی اطلاعاتی که در برنامه وارد کرده اید را از دست خواهید داد.",
			"Finish": "تمام",
			"An error occured, please check inputs and try again.": "خطایی پیش آمد، لطفا ورودی ها را بررسی کنید و دوباره تلاش کنید.",
			"Please specify a password to protect your data.": "لطفا یک کلمه عبور برای محافظت از اطلاعات خود وارد کنید.",
			"Passwords don't match. Try again?": "کلمات عبور یکی نیستند. دوباره امتحان کنید؟",
			"Please enter the master password for this database to access all it's contents.": "لطفا برای دستیابی به محتوای فایل انتخاب شده کلمه عبور مربوطه را وارد کنید.",
			"Enter Master Password": "کلمه عبور اصلی را وارد کنید",
			"Please enter master password of the selected database.": "لطفا کلمه عبور اصلی دیتابیس انتخاب شده را وارد کنید.",
			"Password is wrong or the selected database file is invalid!": "کلمه عبور اشتباه است و یا فایل انتخاب شده معتبر نمی باشد!",
			"Short passwords are easy to guess. Try one with at least 8 characters. How about a sentence?": "کلمات عبور کوتاه به راحتی حدس زده می شوند. حداقل از 8 حرف استفاده کنید. چطوره یه جمله انتخاب کنید؟",
			"Specified file does not exist!": "فایل مشخص شده وجود ندارد!",
			"This password would get cracked in less than a second! Please choose a different one.": "این کلمه عبور در کمتر از یک ثانیه شکسته می شود! لطفا کلمه عبور دیگری انتخاب کنید.",
			"You haven't configured Jakilid yet. Please run Jakilid from your desktop shortcut or Start menu and configure it.": "شما تا به حال تنظیمات جاکلید را انجام نداده اید. لطفا جاکلید را از روی میز کار خود و یا منوی شروع اجرا و تنظیم کنید.",
			"Jakilid is not configured": "جاکلید تنظیم نیست"
		},
		"account": {
			"Enter info title": "عنوان را وارد کنید",
			"Add labels": "دسته‌بندی را مشخص کنید",
			"Copy value": "کپی کردن اطلاعات",
			"Drag up or down": "برای مرتب کردن به بالا یا پایین درگ کنید",
			"Choose info type": "نوع اطلاعات را انتخاب کنید",
			"Change info type": "تغییر نوع اطلاعات",
			"Delete info": "حذف اطلاعات",
			"%1 at %2": "%1 در %2",
			"Edit": "ویرایش",
			"Close": "ببند",
			"New Info": "اطلاعات جدید",
			"Save": "ذخیره",
			"Enter value": "اطلاعات را وارد کنید",
			"Enter title": "عنوان اطلاعات را وارد کنید",
			"Username": "نام کاربری",
			"Password": "کلمه عبور",
			"Login URL": "صفحه ورود",
			"Item Details": "جزئیات اطلاعات",
			"Tags": "برچسب ها"
		},
		"labels": {
			"Email": "ایمیل",
			"Bank": "بانک",
			"Social": "شبکه‌های اجتماعی",
			"Personal": "شخصی",
			"Work": "کار",
			"Network": "شبکه",
			"Note": "یادداشت",
			"seperators": "،"
		},
		"form" : {
			"password" : "کلمه عبور",
			"pin": "رمز",
			"pwd" : "کلمه عبور",
			"email": "ایمیل",
			"e-mail": "ایمیل",
			"email address": "ایمیل",
			"mail" : "ایمیل",
			"address": "آدرس",
			"tell": "تلفن",
			"phone": "تلفن",
			"phone number": "شماره تلفن",
			"mobile": "تلفن همراه",
			"username" : "نام کاربری",
			"user" : "نام کاربری",
			"login" : "نام کاربری",
			"id": "شناسه",
			"uid": "نام کاربری",
			"name" : "نام",
			"first name": "نام",
			"last name": "نام خانوادگی",
			"firstname": "نام",
			"lastname": "نام خانوادگی",
			"fullname": "نام کامل",
			"title": "عنوان",
			"author": "نویسنده",
			"description": "توضیحات",
			"zip": "کد پستی",
			"zipcode": "کد پستی",
			"postalcode": "کد پستی",
			"birthday": "روز تولد",
			"birthyear": "سال تولد",
			"city": "شهر",
			"displayname": "نام نمایشی",
			"homepage": "صفحه خانگی",
			//you could add very specific and known form input names in your locale here (lowercase)
			"my e-mail address is" /* amazon */: "ایمیل",
			"email or phone" /* facebook */ : "ایمیل",
			"email or user id" /* ebay */ : "ایمیل",
			"username or email" /* github */ : "نام کاربری"
		},
		"formFill" : {
			"نام کاربری": [
				"username",
				"login",
				"userid",
				"user",
				"karbar",
				"email"
			],
			"کلمه عبور" : [
				"password",
				"ramz"
			],
			"پسورد": [
				"password",
				"ramz"
			],
			"رمز": [
				"pin",
				"password",
				"ramz"
			],
			"ایمیل" : [
				"email",
				"username",
				"login"
			],
			"پست الکترونیک": [
				"email",
				"username",
				"login"
			]
		},
		"addon": {
			"Enter Master Password": "کلمه عبور اصلی را وارد کنید",
			"Unlock": "بازکن",
			"Would you like to save this login information?": "آیا می خواهید اطلاعات این سایت را ذخیره کنید؟",
			"You would be able to auto fill this form by saving it now.": "با ذخیره کردن اطلاعات بعدا می توانید به صورت خودکار فرم را پر کنید.",
			"Show Jakilid": "جاکلید را نمایش بده",
			"Lock": "قفل کن",
			"Click to unlock Jakilid": "برای باز کردن قفل جاکلید کلیک کنید",
			"Click to open Jakilid in new tab": "برای نمایش جاکلید در تب جدید کلیک کنید",
			"Save": "ذخیره کن",
			"Details": "جزئیات را نمایش بده",
			"Not now": "الان نه",
			"Wrong Password!": "کلمه عبور اشتباه است!",
			"Click to auto fill '%1'": "برای پر کردن '%1' کلیک کنید",
			"Auto fill '%1'": "'%1' را پر کن"
		},
		"keytype": {
			"Text": "متن",
			"Password": "کلمه عبور",
			"Note": "یادداشت",
			"Email": "ایمیل",
			"URL": "صفحه وب",
			"Number": "شماره",
			"Tell": "تلفن",
			"Tags": "برچسب"
		}
	},
	gregorian_to_jalali: function(g_y,g_m,g_d)
	{
		g_y = parseInt(g_y);
		g_m = parseInt(g_m);
		g_d = parseInt(g_d);

		var d_4=g_y%4,
			g_a = [0,0,31,59,90,120,151,181,212,243,273,304,334],
			doy_g = g_a[parseInt(g_m)] + g_d,
			jy,doy_j,jm,jd;
		if (d_4==0 && g_m>2)
			doy_g++;
		var d_33 = parseInt( ((g_y-16) % 132) * 0.0305),
			a = (d_33==3 || d_33 < (d_4-1) || d_4==0) ? 286 : 287,
			b = ((d_33==1 || d_33==2) && (d_33==d_4 || d_4==1)) ? 78: ((d_33==3 && d_4==0)?80:79);
		if (parseInt((g_y-10)/63)==30)
		{
			a--;
			b++;
		}
		
		if (doy_g>b)
		{
			jy=g_y-621;
			doy_j=doy_g-b;
		}
		else
		{
			jy=g_y-622;
			doy_j=doy_g+a;
		}
		
		if (doy_j<187)
		{
			jm=parseInt((doy_j-1)/31);
			jd=doy_j-(31*jm++);
		}
		else
		{
			jm=parseInt((doy_j-187)/30);
			jd=doy_j-186-(jm*30);
			jm+=7;
		}
		return {year:jy,month:jm,day:jd};
	}
};


/*!
 * Javascript Jalali date conversion functions by Farshad Shahbazi
 * Note: converted from php functions at http://jdf.scr.ir/
 */
/*
function jalali_to_gregorian(j_y, j_m, j_d)
{
	j_y = parseInt(j_y);
	j_m = parseInt(j_m);
	j_d = parseInt(j_d);
	
	var d_4 = (j_y+1) % 4,
		doy_j = (j_m<7) ? ((j_m-1)*31)+j_d : ((j_m-7)*30) + j_d + 186,
		d_33 = parseInt(((j_y-55)%132) * 0.0305),
		a = (d_33!=3 && d_4<=d_33) ? 287 : 286,
		b = ((d_33==1 || d_33==2) && (d_33==d_4 || d_4==1)) ? 78 : ((d_33==3 && d_4==0) ? 80:79),
		gy,gd,gm,
		ar = [0,31,(gy%4==0)?29:28,31,30,31,30,31,31,30,31,30,31];
	if (parseInt((j_y-19)/63)==20)
	{
		a--;
		b++;
	}
	
	if (doy_j <= a)
	{
		gy = j_y + 621;
		gd = doy_j+b;
	}
	else
	{
		gy = j_y + 622;
		gd = doy_j - a;
	}
	
	for (gm=0; gm<13; ++gm)
	{
		var v = ar[gm];
		if (gd <= v)
			break;
		gd-=v;
	}
	return {year:gy,month:gm,day:gd};
}
*/
