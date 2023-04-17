const menu = document.querySelector(".menu");
const menuItems = document.querySelectorAll(".menuItem");
const hamburger= document.querySelector(".hamburger");
const closeIcon= document.querySelector(".closeIcon");
const menuIcon = document.querySelector(".menuIcon");

function toggleMenu() {
  if (menu.classList.contains("showMenu")) {
    menu.classList.remove("showMenu");
    closeIcon.style.display = "none";
    menuIcon.style.display = "block";
  } else {
    menu.classList.add("showMenu");
    closeIcon.style.display = "block";
    menuIcon.style.display = "none";
  }
}

hamburger.addEventListener("click", toggleMenu);

menuItems.forEach( 
  function(menuItem) { 
    menuItem.addEventListener("click", toggleMenu);
  }
)



function toggleCommands(element) {
    const commandContainer = element.querySelector('.commands-container');
    commandContainer.classList.toggle('hidden');
    if (commandContainer.style.maxHeight === '0px' || !commandContainer.style.maxHeight) {
        commandContainer.style.maxHeight = commandContainer.scrollHeight + 'px';
    } else {
        commandContainer.style.maxHeight = '0';
    }
}
