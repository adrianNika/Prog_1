
let toggleButton = document.getElementById('login-toggle');
let classLogin = document.querySelector('.login');
let classInscription = document.querySelector('.inscription');

    toggleButton.addEventListener('click', () => {
        if (getComputedStyle(classLogin).display == 'none' || classLogin.style.display == 'none') {
            toggleButton.innerText = 'Inscrire';
            classInscription.style.display = 'none';
            classLogin.style.display = 'flex';
        } else {
            toggleButton.innerText = 'Login';
            classLogin.style.display = 'none';
            classInscription.style.display = 'flex';
        }
    });