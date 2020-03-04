-- {{{ Premable and constants
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

    -- Write to index
    local nowdesc = string.format("# [%s](%s)\n%s\n",
        catstr(meta.title), basePath:gsub('.md$', '.html'),
        catstr(meta.description))
    io.output("index/" .. basePath):write(nowdesc)

    return elem
end
-- }}}

-- vim: fdm=marker

