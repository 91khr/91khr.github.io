
var indexctnt, tmp_indexhint;
var nowindex = 0, maxindex;
var indextype = 'index'
var active_controls = {}

function get_index_path() {
    let basepath = (new URL(document.URL)).pathname.replace(/[^/]*$/, '')
    return '/out/index/' + basepath.slice('/out/'.length) + indextype + '.'
}

function set_index_type(type) {
    if (type == indextype)
        return;
    active_controls[indextype].classList.remove('page-control-active')
    active_controls[type].classList.add('page-control-active')
    indextype = type;
    nowindex = 0;
    init_index_content();
}

function updateindex() {
    tmp_indexhint.innerHTML = `'(${nowindex} #:in ${maxindex})`;
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
    function init() {
        active_controls.index = document.getElementById('control-set-to-index')
        active_controls.catalog = document.getElementById('control-set-to-catalog')
        active_controls.index.classList.add('page-control-active')
        init_index_content();
    }
    if (document.readyState != 'loading') {
        init();
    } else {
        document.addEventListener('DOMContentLoaded', init);
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

