/*
	Ripple effect
	--------------------
	Draws a background radial gradient on the clicked element around the event mouse coordinates,
	then it uses a RAF loop to expand the gradients radius untill it reaches 100%
	
	improvments:
	- get background colour - (convert to hsl ?) - use that as a base with the highlight being lighter or darker
*/

const ripple = el => {
	let d = el.dataset.ripple.split('-'),
		 s = 'rgba(3,149,229,0.4)',
		 e = 'rgba(3,149,229,1)';
	d[2] = Number(d[2]) + 4;
	el.dataset.ripple = d.join('-');
	el.style.background = `radial-gradient(circle at ${d[0]}px ${d[1]}px, ${s} 0%, ${s} ${d[2]}%, ${e} ${d[2] + 0.1}%)`;
	
	window.requestAnimationFrame(() => {
		if (el.dataset.ripple && d[2] < 100) ripple(el)
	});
};

const start = ev => {
	ev.target.dataset.ripple = `${ev.offsetX}-${ev.offsetY}-0`
	ripple(ev.target);
};

const stop = ev => {
	let el = document.querySelector('[data-ripple]');
	delete el.dataset.ripple;
	el.style.background = 'none';
};

// Events
document.querySelectorAll('.highlight').forEach( el => el.addEventListener( 'mousedown', start ));
document.addEventListener( 'mouseup', stop );