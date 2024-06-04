const chatForm = document.getElementById('chatForm');
const messageInput = document.getElementById('messageInput');
const chatBox = document.getElementById('chatBox');
const userList = document.getElementById('userList');
const usernameDisplay = document.getElementById('usernameDisplay');
const users = [];

    chatForm.addEventListener('submit', function(event) 
    {
        event.preventDefault();
        const message = messageInput.value.trim();
        if (message) 
        {
            sendMessage(message);
            messageInput.value = '';
        }
    });

    function sendMessage(message) {
        // Envoi du message à tous les utilisateurs connectés (simulé ici)
        for (const user of users) {
            displayMessage(user, message);
        }
    }

    function sendMessageJoinChat(message) 
    {
        // Envoi du message à tous les utilisateurs connectés (simulé ici)
        for (const user of users) 
        {
            displayMessageJoinChat(user, message, true);
        }
    }

    function displayMessage(sender, message) 
    {
        const messageElement = document.createElement('p');
        messageElement.textContent = `${sender}: ${message}`;
        chatBox.appendChild(messageElement);
        // Fait défiler vers le bas pour afficher les nouveaux messages
        chatBox.scrollTop = chatBox.scrollHeight;
    }

    function displayMessageJoinChat(sender, message, isnotif = false) 
    {
        const messageElement = document.createElement('h3');
        messageElement.textContent = `${message}`;
        if (isnotif) 
        {
            messageElement.classList.add('notification');
        }
        chatBox.appendChild(messageElement);
        // Fait défiler vers le bas pour afficher les nouveaux messages
        chatBox.scrollTop = chatBox.scrollHeight;
    }

    // Fonction pour mettre à jour la liste des utilisateurs
    function updateUserList() 
    {
        userList.innerHTML = '';
        for (const user of users) 
        {
            const listItem = document.createElement('li');
            const listPointVert = document.createElement('span');
            const displayUsername = document.createElement('p');
            displayUsername.innerHTML(user);
            userList.appendChild(listPointVert);
            userList.appendChild(listItem);
        }
    }

    let username = '';
    while (!username || username.length > 15 || username.length <= 3 || username.includes(' '))
    {
        username = prompt('Entrez votre nom d\'utilisateur : \n - Min 4 caractères\n - Max 15 caractères\n - Ne dois pas contenire d\'escpace');
    }

    usernameDisplay.textContent = username;
    users.push(username);
    updateUserList();
    sendMessageJoinChat(`${username} a rejoint le chat.`, true);


    let toggle = () => 
    {
        let element = document.getElementById("listhide");
        let hidden = element.getAttribute("hidden");
        if (hidden) 
        {
            element.removeAttribute("hidden");
        } 
        else
        {
            element.setAttribute("hidden", "hidden");
        }
    }