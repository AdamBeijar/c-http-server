let count = 0;

const handle_click = () => {
    count += 1;
    document.getElementById('count').innerText = count;
}

document.getElementById('addOne').addEventListener('click', handle_click);