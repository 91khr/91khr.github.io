-- There's no subdirs currently, so only list all files
local base = ...
local files = list_dir(base)
local articles = { base .. "index.md" }
local tailpat ="(([^/]+).lagda.md)$"

local function proc(f)
    if os.getenv("DEBUG") then
        articles[#articles + 1] = f.path
    else
        local target = f.path:gsub(tailpat, "%2.output.md")
        if need_rebuild(f.path, target) then
            os.execute("cd " .. base ..
                " && agda --html-dir=inter --html --html-highlight=code " ..
                f.path:match("([^/]+.lagda.md)$"))
            os.rename(f.path:gsub(tailpat, "inter/%2.md"), target)
        end
        if file_exists(target) then articles[#articles + 1] = target end
    end
end

for _, f in pairs(files) do
    if f.type == "file" and f.path:match(".lagda.md$") then
        proc(f)
    end
end

return { articles = articles, build_args = "--lua-filter=" .. base .. "/preproc.lua " .. default_build_args }
