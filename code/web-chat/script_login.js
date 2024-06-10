
let toggleButton = document.getElementById('login-toggle');
let classLogin = document.querySelector('.login');
let classInscription = document.querySelector('.inscription');
let submitInput = document.getElementById('submit-input');
let msgAccount = document.getElementById('msg-account');

let buttonVisibleMdp = document.getElementById('btnVisible');
let iconMdp = document.querySelector('.bp-visible i');
let inputPassword1 = document.getElementById('input-type1');
let inputPassword2 = document.getElementById('input-type2');

    toggleButton.addEventListener('click', () => {
        if (getComputedStyle(classLogin).display == 'none' || classLogin.style.display == 'none') 
        {
            toggleButton.innerText = 'S\'inscrire';
            submitInput.innerText = 'Se connecter';
            msgAccount.innerText = 'Vous n\'avez pas de compte ?';
            classInscription.style.display = 'none';
            classLogin.style.display = 'flex';
        } 
        else
        {
            toggleButton.innerText = 'Se connecter';
            submitInput.innerText = 'S\'inscrire';
            msgAccount.innerText = 'Vous avez déjà un compte ?';
            classLogin.style.display = 'none';
            classInscription.style.display = 'flex';
        }
    });


    buttonVisibleMdp.addEventListener('click', () => {
        if  (iconMdp.className == 'fa-solid fa-eye-slash')
        {
            iconMdp.className = 'fa-solid fa-eye';
            inputPassword1.type = 'password';
            inputPassword2.type = 'password';
        }
        else
        {
            iconMdp.className = 'fa-solid fa-eye-slash';
            inputPassword1.type = 'text';
            inputPassword2.type = 'password';
        }
    });