
let toggleButton = document.getElementById('login-toggle');
let classLogin = document.querySelector('.login');
let classInscription = document.querySelector('.inscription');
let submitInput = document.getElementById('submit-input');
let msgAccount = document.getElementById('msg-account');

let buttonVisibleMdp = document.querySelectorAll('.bp-visible');
let iconMdp = document.querySelectorAll('.bp-visible i');
let inputPassword = document.querySelectorAll('.inputMdp');

let submitRegister = document.getElementById('submit-input-register');
let loginToggle = document.getElementById('login-toggle');

const fileUpload = document.getElementById('fileUpload');
const profilePic = document.getElementById('profilePic');


    toggleButton.addEventListener('click', () => {
        if (getComputedStyle(classLogin).display == 'none' || classLogin.style.display == 'none') 
        {
            loginToggle.innerHTML = 'S\'inscrire';
            msgAccount.innerText = 'Vous n\'avez pas de compte ?';
            classInscription.style.display = 'none';
            classLogin.style.display = 'flex';
        } 
        else
        {
            loginToggle.innerHTML = 'Se connecter';
            msgAccount.innerText = 'Vous avez déjà un compte ?';
            classInscription.style.display = 'flex';
            classLogin.style.display = 'none';
        }
    });

    for (let i = 0; i < buttonVisibleMdp.length; i++) {
        
        buttonVisibleMdp[i].addEventListener('click', () => {
            if  (iconMdp[i].className == 'fa-solid fa-eye-slash')
            {
                iconMdp[i].className = 'fa-solid fa-eye';
                inputPassword[i].type = 'password';
            }
            else
            {
                iconMdp[i].className = 'fa-solid fa-eye-slash';
                inputPassword[i].type = 'text';
            }
        });
    }

    fileUpload.addEventListener('change', function(event) {
        const file = event.target.files[0];
        if (file) {
            const reader = new FileReader();
            reader.onload = function(e) {
                profilePic.style.backgroundImage = `url(${e.target.result})`;
            };
            reader.readAsDataURL(file);
        }
    });


    submitInput.addEventListener('submit', (teazix_NTM_EVENT) => {
        teazix_NTM_EVENT.preventDefault();
    });



