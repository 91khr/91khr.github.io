-- {{{ Premable and constants
local os = require('os')

local indexSize = 1926
local basePath = os.getenv("basePath")
local baseLen = (function()
    local res = -1
    local last = 0
    repeat
        last = basePath:find('/', last + 1)
        res = res + 1
    until last == nil
    return res
end)()
-- }}}

-- {{{ Helpers
local function catstr(elem)
    local function runinto(elem)
        if elem.text then
            return elem.text
        elseif elem.content then
            local res = ''
            for _, v in pairs(elem.content) do
                res  = res .. catstr(v) .. ' '
            end
            return res
        end
    end
    if type(elem) == type('') then
        return elem
    end
    local res = ''
    for _, v in pairs(elem or {}) do
        res = res .. (runinto(v) or '') .. ' '
    end
    return res
end
-- }}}

-- {{{ main
function Pandoc(elem)
    local meta = elem.meta

    if not meta.title then
        if basePath == 'index.html' then  -- Main index
            meta.main = true
            meta.title = 'Senioria的刺猬洞'
        elseif os.getenv("alterIndex") then  -- Alternative index
            meta.title = basePath:gsub('[^/]*$', '')
            local placeholder = pandoc.Div({pandoc.Null()})
            placeholder.classes = { "index-nil-introduction" }
            elem.blocks:insert(placeholder)
        end
    end

    -- Write to index
    local nowdesc = string.format("<article><span class='article-title'><a href=\"%s\">%s</a></span>\n" ..
        "<span class='article-description'>%s</span></article>\n",
        "/out/" .. basePath, catstr(meta.title),
        catstr(meta.description))
    io.output("index/" .. basePath):write(nowdesc)

    return elem
end
-- }}}

-- {{{ Auto add line numbers
function CodeBlock(elem)
    local len = select(2, string.gsub(elem.text, '\n', '')) + 1
    elem.attributes['style'] = (elem.attributes['style'] or '') ..
        "--linenumber-len: " .. tostring(tostring(len):len()) .. ';'
    return elem
end
-- }}}

-- vim: fdm=marker

