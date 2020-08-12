Jakilid.prototype = Hendoone;	//inherit
Jakilid.prototype.constructor = Jakilid;	//override constructor to connect signals
var JakilidWrapper = new Jakilid();

function Jakilid()
{
	this.onLock = null;
	this.onUnlock = null;
	var self = this;
	
	Hendoone.locked.connect(function(){self.onLock();});
	Hendoone.unLocked.connect(function(){self.onUnlock();});	
}

/*
Jakilid.prototype.find = function(what, index, offset, callBack)
{
	var ret = Hendoone.prototype.find.call(this, what, index, offset);
	if (typeof callBack === 'function')
		callBack(ret);
	return true;
}
*/
