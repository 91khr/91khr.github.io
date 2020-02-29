
var indexctnt;
var nowindex = 0, maxindex;

function nextindex() {
    if (nowindex == maxindex)
        return;
    nowindex = nowindex + 1;
    fetchCtnt("index/" + nowindex + ".html", ctnt => indexctnt.innerHTML = ctnt);
}

function previndex() {
    if (nowindex == 1)
        return;
    nowindex = nowindex - 1;
    fetchCtnt("index/" + nowindex + ".html", ctnt => indexctnt.innerHTML = ctnt);
}

function initindex() {
    function doinit() {
        fetchCtnt("index.count.txt", count => {
            maxindex = count
            // TODO: Init the index list
            document.getElementById('index-control').innerHTML += '一共有' + maxindex + '页'
            indexctnt = document.getElementById('index-content')
            nextindex();
        });
    }

    if (document.readyState != 'loading'){
        doinit();
    } else {
        document.addEventListener('DOMContentLoaded', doinit);
    }
}

function fetchCtnt(url, callback) {
    let request = new XMLHttpRequest()
    request.open('GET', url)
    request.responseType = 'text'
    request.onload = function() {
        callback(request.response)
    }
    request.send()
}

