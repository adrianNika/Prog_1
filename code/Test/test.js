document.getElementById('toggleButton').addEventListener('click', function() {
    const div1 = document.getElementById('div1');
    const div2 = document.getElementById('div2');

    if (div1.classList.contains('hidden')) {
        // div1 est actuellement caché, on montre div1 et cache div2 après l'animation
        div2.classList.add('hidden');
        div2.addEventListener('transitionend', function handler() {
            div2.removeEventListener('transitionend', handler);
            div1.classList.remove('hidden');
        });
    } else {
        // div1 est actuellement visible, on montre div2 et cache div1 après l'animation
        div1.classList.add('hidden');
        div1.addEventListener('transitionend', function handler() {
            div1.removeEventListener('transitionend', handler);
            div2.classList.remove('hidden');
        });
    }
});
