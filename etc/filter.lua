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
local defaultVars = {
    description = (function()
        local elem = pandoc.Span({pandoc.Str(' ')})
        elem.classes = { "replized-content" }
        return pandoc.MetaInlines({elem})
    end)(),
    content_prompt = pandoc.MetaInlines({ pandoc.Str("(display content)") }),
}
-- }}}

-- {{{ Helpers
local function catstr(elem)
    if type(elem) ~= 'table' then
        return elem
    end
    local function escape(s, in_attribute)
        return (type(s) == 'table' and s.text or s):gsub("[<>&\"']",
        function(x)
            return ({
                ['>']='&gt;', ['<']='&lt;', ['&']='&amp;', ['"']='&quot;', ['\'']='&#39;'
            })[x] or x
        end)
    end
    local res = ''
    local filter = {
        Emph = function(s) return pandoc.Str('<em>' .. catstr(s.content) .. '</em>') end,
        Strikeout = function(s) return pandoc.Str('<del>' .. catstr(s.content) .. '</del>') end,
        Strong = function(s) return pandoc.Str("<strong>" .. catstr(s.content) .. "</strong>") end,
        Subscript = function(s) return pandoc.Str("<sub>" .. catstr(s.content) .. "</sub>") end,
        Superscript = function(s) return pandoc.Str("<sup>" .. catstr(s.content) .. "</sup>") end,
        SmallCaps = function(s)
            return pandoc.Str('<span style="font-variant: small-caps;">' .. catstr(s.content) .. '</span>')
        end,
        Link = function(s)
            return pandoc.Str("<a href='" .. escape(s.target,true) .. "'>" .. catstr(s.content) .. "</a>")
        end,
        Str = function(s) return pandoc.Str(escape(s)) end,
        Space = function() return pandoc.Str(" ") end,
        Span = function(s)
            local res = '<span class="'
            for _, v in pairs(s.classes) do
                res = res .. ' ' .. v
            end
            res = res .. '">' .. catstr(s.content) .. '</span>'
            return pandoc.Str(res)
        end,
    }
    for k, v in pairs(elem) do
        res = res .. (pandoc.walk_inline(pandoc.Span {v}, filter).content[1].text or '')
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
        end
    end
    for k, v in pairs(defaultVars) do
        if not meta[k] then
            meta[k] = v
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

