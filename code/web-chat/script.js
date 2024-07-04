const form = document.getElementById('chatForm');
const input = document.getElementById('messageInput');
const messages = document.getElementById('chatBox');
const usernameDisplay = document.getElementById('usernameDisplay');
const buttonListUser = document.querySelector('.online-button');
let userDiv = document.querySelector('.user');
let iconArrow = document.querySelector('.online-button i');
const imgPP = document.getElementById('pp');
const storedUser = JSON.parse(localStorage.getItem('user'));
const userUsername = storedUser.username;
usernameDisplay.textContent = storedUser.username;
const socket = io();

let incr = 0;
    
    form.addEventListener('submit', (e) => {
        e.preventDefault();
        if (input.value) 
        {
            socket.emit('chat message', {sender: userUsername, message: input.value});
            input.value = '';
        }
    });

    socket.on('chat message', (data) => {
        const item = document.createElement('div');
        item.id = 'itemSend';

        const itemPP = document.createElement('div');
        itemPP.id = 'itemPP';

        const itemMsg = document.createElement('div');
        itemMsg.id = 'itemMsg';

        const pp = document.createElement('img');
        pp.id = 'pp';
        pp.src = '/image/font.jpg';
        itemPP.appendChild(pp);

        const itemUser = document.createElement('p');
        itemUser.id = 'itemUser';
        itemUser.textContent = data.sender;

        const itemContent = document.createElement('p');
        itemContent.id = 'itemContent';
        itemContent.textContent = data.message;

        itemMsg.appendChild(itemUser);
        itemMsg.appendChild(itemContent);

        item.appendChild(itemPP);
        item.appendChild(itemMsg);
        messages.appendChild(item);
    });

    socket.emit('register', {userUsername});

    // Fonction pour mettre à jour la liste des utilisateurs

    function updateUserList(user) {
        const listItem = document.createElement('li');
        const listPointVert = document.createElement('span');
        const displayUsername = document.createElement('p');
        displayUsername.textContent = user;
        listItem.appendChild(listPointVert);
        listItem.appendChild(displayUsername);
        userList.appendChild(listItem);
    }


    socket.on('update list', function(users) {
        let userList = document.getElementById('userList');
        userList.innerHTML = '';
        users.forEach(user => {
            updateUserList(user);
        });
    });
    
    
    buttonListUser.addEventListener('click', () =>  {
        if (userDiv.classList.contains("activeListUser")) { 
            userDiv.classList.remove("activeListUser");
            iconArrow.style.transform = "rotate(0deg)";
        } else {
            userDiv.classList.add("activeListUser");
            iconArrow.style.transform = "rotate(180deg)";
        }
    });