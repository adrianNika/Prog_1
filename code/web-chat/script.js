const form = document.getElementById('chatForm');
const input = document.getElementById('messageInput');
const messages = document.getElementById('chatBox');
const userList = document.getElementById('userList');
const usernameDisplay = document.getElementById('usernameDisplay');
const buttonListUser = document.querySelector('.online-button');
let userDiv = document.querySelector('.user');
let iconArrow = document.querySelector('.online-button i');
const users = [];
const socket = io();

    form.addEventListener('submit', (e) => {
        e.preventDefault();
        if (input.value) {
        socket.emit('chat message', input.value);
        input.value = '';
        }
    });

    socket.on('chat message', (msg) => {
        const item = document.createElement('p');
        item.textContent = `${users}: ${msg}`;
        messages.appendChild(item);
    });

    const toggleButton = document.getElementById('toggle-btn');

    toggleButton.addEventListener('click', (e) => {
        e.preventDefault();
        if (socket.connected) {
        toggleButton.innerText = 'Connect';
        socket.disconnect();
        } else {
        toggleButton.innerText = 'Disconnect';
        socket.connect();
        }
    });

    // Fonction pour mettre à jour la liste des utilisateurs
    function updateUserList() 
    {
        userList.innerHTML = '';
        for (const user of users)
        {
            const listItem = document.createElement('li');
            const listPointVert = document.createElement('span');
            const displayUsername = document.createElement('p');
            displayUsername.textContent = user;
            listItem.appendChild(listPointVert);
            listItem.appendChild(displayUsername);
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

    
    buttonListUser.addEventListener('click', () =>  {
        if (userDiv.classList.contains("activeListUser")) { 
            userDiv.classList.remove("activeListUser");
            iconArrow.style.transform = "rotate(0deg)";
        } else {
            userDiv.classList.add("activeListUser");
            iconArrow.style.transform = "rotate(180deg)";
        }
    });

    
