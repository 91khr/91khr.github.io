-- {{{ Premable and constants
local os = require('os')

local basePath = os.getenv("basePath")
local defaultVars = {
    content_prompt = pandoc.MetaInlines({ pandoc.Str("(display content)") }),
}
-- }}} End premable

-- {{{ main
function Pandoc(elem)
    local meta = elem.meta

    if not meta.title then
        if basePath == 'index.html' then  -- Main index
            meta.main = true
            meta.title = 'Senioria的刺猬洞'
        elseif os.getenv("alterIndex") or basePath:match('index.html$') then  -- Index
            meta.title = basePath:gsub('[^/]*$', '')
        else  -- Normal article
            meta.title = "无题"
        end
    end
    for k, v in pairs(defaultVars) do
        if not meta[k] then
            meta[k] = v
        end
    end

    -- Write to index
    local desctag = ''
    if meta.description then
        desctag = "<span class='article-description'>" ..
                  pandoc.write(pandoc.Pandoc(meta.description or ''), 'html') ..
                  "</span>"
    end
    local nowdesc = string.format(
        "<article><span class='article-title'><a href=\"%s\">%s</a></span>\n%s</article>\n",
        "/" .. basePath,
        pandoc.write(pandoc.Pandoc(meta.title), 'html'),
        desctag)
    io.output("index/" .. basePath):write(nowdesc)

    return elem
end
-- }}} End main

-- {{{ Auto add line numbers
function CodeBlock(elem)
    local len = select(2, string.gsub(elem.text, '\n', '')) + 1
    elem.attributes['style'] = (elem.attributes['style'] or '') ..
        "--linenumber-len: " .. tostring(tostring(len):len()) .. ';'
    return elem
end
-- }}} End line numbers

-- {{{ Header translations
function Header(elem)
    elem.level = elem.level + 1
    return elem
end
-- }}} End header translations

-- vim: fdm=marker

