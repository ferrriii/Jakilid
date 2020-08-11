/* PasswdStrength.js version 1.0
 * Copyright 2015 Farshad Shahbazi
 * Released under the MIT license
 * https://github.com/Jakilid/PasswdStrength.js
*/
function passStrength(p, averageEntropy)
{
	/* assume entropy 55 as 50% strong password by default.
	 * according to average cracking speed based on http://hashcat.net/oclhashcat/ , it takes up to 1 month to crack such a password in 2014. entropy 56 would takes 2 months and so on.
	 * Moore's law says "the number of transistors on a dense integrated circuit doubles approximately every two years",
	 * so it's not a bad idea to increase 55 (averageEntropy) by one every two years passed 2014.
	 */
	 if (!p)
		return 0;
	averageEntropy = averageEntropy || 55;
	function log10(val)
	{
		return Math.log(val) / Math.LN10;
	}


	function knownPatterns(p)
	{
		var n= 0, match;
		var patt = /123|234|345|456|567|678|789|890|abc|bcd|cde|def|efg|fgh|ghi|hij|ijk|jkl|klm|lmn|mno|nop|opq|pqr|qrs|rst|stu|tuv|uvw|vwx|wxy|xyz|qwe|qaz|wsx|zaq|xsw|asd|zxc|321|432|543|654|765|876|987|098|852|^[a-z][aeiouy]|(.)\1\1|\d$/igm;
		while (match = patt .exec(p))
			n++;//n += match[0].length;
		n += /^[A-Z]/gm.test(p) ? 1 : 0;	//pattern:starting with upper case
		
		return n;
	}

	var l=p.length;
	var n=/[0-9]/.test(p) ? 10 : 0;
	n+=/[a-z]/.test(p) ? 26 : 0;
	n+=/[A-Z]/.test(p) ? 26 : 0;
	n+=/[!"#$%&'()*+,-./:;<=>?@[\]^_`{|}~]/.test(p) ? 32 : 0;
	n+=/[^\u0000-\u007F]/.test(p) ? 128 : 0;		//non-ASCII
	n+=/ /.test(p) ? 1 : 0;

	l -= knownPatterns(p);
	//l= l- dicWordLength(p);// || dicwordLen(a->@)

	//http://en.wikipedia.org/wiki/Password_strength
	var h = l * 3.32193 *  log10(n);	//entropy, 3.321 = 1/log10(2)


	/* averageEntropy is assumed as 50% strong.
	 * any entropy below that is exponentially weaker and any entropy above that is exponentially stronger.
	 */
	return Math.exp(h * (Math.log(50) / averageEntropy)) ;	
}