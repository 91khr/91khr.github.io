
var indexctnt, tmp_indexhint;
var nowindex = 0, maxindex;

function updateindex() {
    tmp_indexhint.innerHTML = `([page ${nowindex}] [maxpage ${maxindex}])`;
    fetchCtnt("index/" + nowindex + ".html", ctnt => indexctnt.innerHTML = ctnt);
}

function nextindex() {
    if (nowindex == maxindex)
        return;
    nowindex = nowindex + 1;
    updateindex();
}

function previndex() {
    if (nowindex == 1)
        return;
    nowindex = nowindex - 1;
    updateindex();
}

function initindex() {
    function doinit() {
        fetchCtnt("index.count.txt", count => {
            maxindex = count
            // TODO: Init the index list
            tmp_indexhint = document.getElementById('temp-index-hint')
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

