function toggleCommands(element) {
    const commandContainer = element.querySelector('.commands-container');
    commandContainer.classList.toggle('hidden');
    commandContainer.style.maxHeight = commandContainer.style.maxHeight === '300px' ? '0' : '300px';
}
