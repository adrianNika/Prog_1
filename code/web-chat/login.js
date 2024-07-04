
let toggleButton = document.getElementById('login-toggle');
let classLogin = document.querySelector('.login');
let classInscription = document.querySelector('.inscription');
let submitInputLogin = document.getElementById('submit-input-login');
let submitInputRegister = document.getElementById('submit-input-register');
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


    submitInputLogin.addEventListener('click', async (teazix_NTM_EVENT) => {
        const username = document.getElementById('username-login').value;
        const password = document.getElementById('password-login').value;

        try
        {
            const response = await fetch('http://localhost:3001/login', { //Appel de la route qui se situe dans le script serveur
                method: 'POST', // Méthode POST pour envoie de donnée | GET pour la récupération de donner
                headers: 
                {
                    'Content-Type': 'application/json' // type de donnée récupèrer, don du JSON
                },
                body: JSON.stringify({ username, password }) // Les données envoyer dans le corps de la requête
            });   
            
            if(response.ok)
            {
                const user = await response.json();

                localStorage.setItem('user', JSON.stringify(user));

                window.location.href = '/chat.html';
            }
            else
            {
                const error = await response.text();
                alert(`Erreur : ${error}`);
            }
        }
        catch(error)
        {
            console.error(`Erreur lors de la tentative de connexion:`, error);
        }
    });


    submitInputRegister.addEventListener('click', async (teazix_NTM_EVENT) => {
        teazix_NTM_EVENT.preventDefault();

        const username = document.getElementById('username').value;
        const email = document.getElementById('email').value;
        const telephone = document.getElementById('tel').value;
        const password = document.getElementById('password').value;

        const response = await fetch('http://localhost:3001/register', { //Appel de la route qui se situe dans le script serveur
            method: 'POST', // Méthode POST pour envoie de donnée | GET pour la récupération de donner
            headers: 
            {
                'Content-Type': 'application/json' // type de donnée récupèrer, don du JSON
            },
            body: JSON.stringify({ username, email, telephone, password }) // Les données envoyer dans le corps de la requête
        });

        if(response.ok)
        {
            window.location.href = '/chat.html';
            alert("inscription réussi");
        }
        else
        {
            alert("inscription échoué");
        }
    });



