local cbid = 0
function RawBlock(elem)
    if elem.format ~= "html" or not elem.text:match('^<pre class="Agda">') then return end
    -- Clip out the default wrappers
    local text = elem.text:gsub('^<pre class="Agda">', '')
    text = text:gsub('</pre>$', '')
    -- Add pandoc wrappers
    cbid = cbid + 1
    local lnid = 0
    local res = ""
    for line in text:gmatch("[^\n]*\n") do
        lnid = lnid + 1
        res = res .. string.format([[<span id="cb%d-%d"><a href="#cb%d-%d"></a>]], cbid, lnid, cbid, lnid) ..
              line:gsub('class="[%a ]-(%a*)">', function(class)
                  return 'class="' .. (({
                      Keyword = "kw",
                      Module = "cn",
                      Symbol = "va",
                      Bound = "va",
                      Datatype = "dt",
                      Number = "fl",
                      InductiveConstructor = "fu",
                      Function = "fu",
                      Operator = "op",
                      Comment = "co",
                      Hole = "pp",
                  })[class] or class) .. '">'
              end):
              gsub('href="([%w._-]*).html(#?%d*)"', function(fname, id)
                  return 'href="' .. fname .. '.output.html' .. id .. '"'
              end) .. "</span>\n"
    end
    res = string.format([[<div class="sourceCode" id="cb%d" style="--linenumber-len: %d;">]],
                cbid, tostring(lnid):len()) ..
          [[<pre class="sourceCode numberSource agda numberLines"><code class="sourceCode agda">]] .. res ..
          [[</code></pre></div>]]
    -- Store back
    elem.text = res
    return elem
end
