
let toggleButton = document.getElementById('login-toggle');
let classLogin = document.querySelector('.login');
let classInscription = document.querySelector('.inscription');
let submitInput = document.getElementById('submit-input');
let msgAccount = document.getElementById('msg-account');

let buttonVisibleMdp = document.getElementById('btnVisible');
let iconMdp = document.querySelector('.bp-visible i');
let inputPassword = document.getElementById('input-type');

let buttonVisibleMdp2 = document.getElementById('btnVisible2');
let iconMdp2 = document.querySelector('.bp-visible2 i');
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
            classInscription.style.display = 'flex';
            classLogin.style.display = 'none';
        }
    });


    buttonVisibleMdp.addEventListener('click', () => {
        if  (iconMdp.className == 'fa-solid fa-eye-slash')
        {
            iconMdp.className = 'fa-solid fa-eye';
            inputPassword.type = 'password';
        }
        else
        {
            iconMdp.className = 'fa-solid fa-eye-slash';
            inputPassword.type = 'text';
        }
    });

    buttonVisibleMdp2.addEventListener('click', () => {
        if  (iconMdp2.className == 'fa-solid fa-eye-slash')
        {
            iconMdp2.className = 'fa-solid fa-eye';
            inputPassword2.type = 'password';
        }
        else
        {
            iconMdp2.className = 'fa-solid fa-eye-slash';
            inputPassword2.type = 'text';
        }
    });