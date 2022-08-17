"use strict";

let indexctnt, tmp_indexhint;
let nowindex = 0, maxindex;
let indextype = 'index'
let active_controls = {}

function get_index_path() {
    let basepath = (new URL(document.URL)).pathname.replace(/[^/]*$/, '')
    return '/index/' + basepath.slice('/'.length) + indextype + '.'
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

async function updateindex() {
    tmp_indexhint.innerHTML = `'(${nowindex} #:in ${maxindex})`;
    let ctnt = await fetch(`${get_index_path()}${nowindex}.html`);
    if (ctnt.ok)
        indexctnt.innerHTML = await ctnt.text();
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

async function init_index_content() {
    let ctnt = await fetch(`${get_index_path()}count.txt`);
    if (!ctnt.ok) {
        maxindex = 0;
        nowindex = 0;
    }
    else {
        maxindex = Number(await ctnt.text());
        nowindex = 1;
    }
    tmp_indexhint = document.getElementById('temp-index-hint');
    indexctnt = document.getElementById('index-content');
    updateindex();
}

function initindex() {
    function init() {
        active_controls.index = document.getElementById('control-set-to-index');
        active_controls.catalog = document.getElementById('control-set-to-catalog');
        active_controls.index.classList.add('page-control-active');
        init_index_content();
    }
    if (document.readyState != 'loading') {
        init();
    } else {
        document.addEventListener('DOMContentLoaded', init);
    }
}

