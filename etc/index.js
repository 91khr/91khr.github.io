
var indexctnt, tmp_indexhint;
var nowindex = 0, maxindex;
var indextype = 'index'

function get_index_path() {
    let basepath = (new URL(document.URL)).pathname.replace(/[^/]*$/, '')
    return '/out/index/' + basepath.slice('/out/'.length) + indextype + '.'
}

function set_index_type(type) {
    if (type == indextype)
        return;
    indextype = type;
    nowindex = 0;
    init_index_content();
}

function updateindex() {
    tmp_indexhint.innerHTML = `'(#:page ${nowindex} #:maxpage ${maxindex})`;
    fetch_ctnt(get_index_path() + nowindex + ".html", ctnt => indexctnt.innerHTML = ctnt);
}

function nextindex() {
    if (nowindex >= maxindex)
        return;
    nowindex = nowindex + 1;
    updateindex();
}

function previndex() {
    if (nowindex <= 1)
        return;
    nowindex = nowindex - 1;
    updateindex();
}

function init_index_content() {
    fetch_ctnt(get_index_path() + "count.txt", count => {
        maxindex = Number(count)
        nowindex = 0
        // TODO: Init the index list
        tmp_indexhint = document.getElementById('temp-index-hint')
        indexctnt = document.getElementById('index-content')
        nextindex();
    });
}
function initindex() {
    if (document.readyState != 'loading'){
        init_index_content();
    } else {
        document.addEventListener('DOMContentLoaded', init_index_content);
    }
}

function fetch_ctnt(url, callback) {
    let request = new XMLHttpRequest()
    request.open('GET', url)
    request.responseType = 'text'
    request.onload = function() {
        callback(request.response)
    }
    request.send()
}

