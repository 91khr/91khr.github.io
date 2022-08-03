-- {{{ Premable and constants
local os = require('os')

local basePath = os.getenv("basePath")
local defaultVars = {
    description = (function()
        local elem = pandoc.Span({pandoc.Str(' ')})
        elem.classes = { "replized-content" }
        return pandoc.MetaInlines({elem})
    end)(),
    content_prompt = pandoc.MetaInlines({ pandoc.Str("(display content)") }),
}
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
        "/out/" .. basePath,
        pandoc.write(pandoc.Pandoc(meta.title), 'html'),
        pandoc.write(pandoc.Pandoc(meta.description), 'html'))
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

