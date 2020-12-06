var c = document.getElementById("myCanvas");
var ctx = c.getContext("2d");

ctx.beginPath();

var R=0.000008, r=0.000001, a=0.000004;
var x0=R+r-a, y0=0;
ctx.moveTo(-118.2891150+10*x0,34.0211765+10*y0);
	document.write("Start");
	document.write("*************************");
	document.write(-118.2891150+10*x0);
	document.write("$");
	document.write(34.0211765+10*y0);
	document.write("\n");

var cos=Math.cos, sin=Math.sin, pi=Math.PI, nRev=16;
for(var t=0.0;t<(pi*nRev);t+=1){
	var x=(R+r)*cos((r/R)*t)-a*cos((1+r/R)*t);
	var y=(R+r)*sin((r/R)*t)-a*sin((1+r/R)*t);
	document.write("*************************");
	document.write(-118.2891150+10*x);
	document.write("$");
	document.write(34.0211765+10*y);
	document.write("\n");
	ctx.lineTo(-118.2891150+10*x,34.0211765+10*y);
}
ctx.stroke();