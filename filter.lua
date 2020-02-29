--[[
--Tasks:
-- - Add the link to index.css to the document
-- - Write abstract to index
--]]
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

local function ptable(t)
    for k, v in pairs(t) do
        print(k, v)
    end
end

function Pandoc(elem)
    local meta = elem.meta
    -- Append css
    meta.css = pandoc.MetaList({
        pandoc.MetaString(string.rep('../', baseLen) .. "index.css")
    })
    -- Write to index
    do
        local maxindex = io.input("out/index.count.txt"):read("*n")
        local oristr = io.input(string.format("index/%d.md", maxindex)):read("*a")
        local nowabstract = string.format("# [%s](%s)\n%s\n\n",
            meta.title[1].text, basePath:gsub('.md$', '.html'), meta.abstract[1].text);
        if nowabstract:len() + oristr:len() > indexSize then maxindex = maxindex + 1 end
        local out = io.open(string.format("index/%d.md", maxindex), "w")
        out:write(nowabstract)
        out:write(oristr)
        out:close()
    end
    return elem
end

