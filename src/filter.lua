local base = ...
local files = list_dir(base)
local articles = {}
local subdirs = {}
for _, file in pairs(files) do
    if file.type == "file" and file.path:match(".md$") then
        articles[#articles + 1] = file.path
    elseif file.type == "dir" and file.path ~= "src/out" then
        subdirs[#subdirs + 1] = file.path
    end
end
return { articles = articles, subdirs = subdirs, unindiced = { "src/test.md", "src/friends.md" } }
