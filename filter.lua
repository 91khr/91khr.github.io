local os = require('os')

local indexSize = 1926

local basePath = os.getenv("basePath")
local baseLen = (function()
    local res = -1
    local last
    repeat
        last = basePath:find('/')
        res = res + 1
    until last == nil
    return res
end)()

local function ptable(t, s)
    print('ptable', s)
    for k, v in pairs(t) do
        print(k, v)
    end
    print('end ptable')
end

local function procindex(meta)
    local maxindex = io.input("out/index.count.txt"):read("*n")
    local oristr = io.input(string.format("index/%d.md", maxindex)):read("*a")
    local nowdesc = string.format("# [%s](%s)\n",
    meta.title[1].text, basePath:gsub('.md$', '.html'))
    local function pdesc(desc)
        if desc.text then
            nowdesc = nowdesc .. desc.text
        elseif desc.content then
            for _, v in pairs(desc.content) do
                pdesc(v)
                nowdesc = nowdesc .. ' '
            end
        end
    end
    for _, v in pairs(meta.description) do
        pdesc(v)
        nowdesc = nowdesc .. ' '
    end
    nowdesc = nowdesc .. '\n\n'

    if nowdesc:len() + oristr:len() > indexSize then
        maxindex = maxindex + 1
    end
    local out = io.open(string.format("index/%d.md", maxindex), "w")
    out:write(nowdesc)
    out:write(oristr)
    out:close()
end

function Pandoc(elem)
    local meta = elem.meta
    local pathprefix = string.rep('../', baseLen)

    -- Append css
    meta.css = pandoc.MetaList({
        pandoc.MetaString(pathprefix .. "index.css")
    })

    -- Write special links
    do
        local special_links = {
            self_intro = "关于Senioria",
            friends = "友情链接",
            index = "主页",
        }
        local res = {}
        for k, v in pairs(special_links) do
            table.insert(res, { addr = pathprefix .. k .. '.html', name = v })
        end
        meta.special_links = res
    end

    -- Write to index
    procindex(meta)

    return elem
end

