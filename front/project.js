var PROJECT = null;
var NODLESS = {};
var TYPELESS = new Map();

var SELECTED_LINK = null;

// Node LINKS reverse map
var eleidReverseMap = new Map(); // Remove one on rm for both

// node reverse map
var eleToNodeAddr = new Map();

async function request_packs() {
    return (await fetch("%pack_rq%")).json()
}

function new_project() {
    createOptionsWindow(false);
}

function createOptionsWindow(bfm) {
    let innerHTML = "";

    innerHTML+="<h3>Details</h3>";
    innerHTML+=`<div><label>Project name    </label><input id="prj.name" value="${bfm ? PROJECT.name : ''}"></div>`;
    innerHTML+=`<div><label>Project author   </label><input id="prj.author" value="${bfm ? PROJECT.author : ''}"></div>`;



    innerHTML+=`<h3>Other</h3>`;
    let innerv = "";
    if (bfm) Object.entries(PROJECT.ext_prefs).forEach((kv) => innerv += `${kv[0]}:${kv[1]}\n`);

    innerHTML+=`<div><textarea id="prj.ext_field" style="width: 200px; height 60px;" placeholder="Place for custom external preferences strings.\n format: key:val" value="${innerv}"></textarea></div>`

    
    innerHTML+=`<button id="prj.construct">${bfm ? "Save" : "Construct"}</button>`
    let winds = create_dwindow("New project", new_dim(1000, 700), innerHTML);

    document.getElementById("prj.construct").onclick = function(event) {
        let exts = {};
        document.getElementById("prj.ext_field").value.split("\n").forEach((line) => {
            let part = line.split(":");
            if (part.length === 2) {
                exts[part[0]] = part[1];
            }
        });

// let lr_config = {
//     name: "Test project 1",
//     author: "qigan",
//     using: [
//         "defpack"
//     ],
//     nodes: [],
//     ext_prefs: {}
// }

        if (bfm) {
            PROJECT.name = document.getElementById("prj.name").value;
            PROJECT.author = document.getElementById("prj.author").value;
            PROJECT.ext_prefs = exts;
        } else {
            let prj_conf = {
                name: document.getElementById("prj.name").value,
                author: document.getElementById("prj.author").value,
                using: [
                    "defpack"
                ],
                nodes: [],
                ext_prefs: exts
            };

            document.getElementById("cm-container").innerHTML = "";
            document.getElementById("wsp").innerHTML = "";

            load_project(prj_conf);
            


            winds.forEach((wind) => document.getElementById(wind).remove());
        }
    }
}

function create_new_node(nodeID) {
    let ptbl_node_info = NODLESS[nodeID];
    return {
        uuid: crypto.randomUUID(),
        id: nodeID,
        ninf: ptbl_node_info,
        prefs: ptbl_node_info.prefs,
        links: []
    }
}

function exclude_rebuild_list(list, ioob) {
    let m = [];
    for (let i = 0; i < list.length; i++) {
        if (i==ioob || list[i] == ioob) continue;
        m.push(list[i]);
    }
    return m;
}

function nodeFromLink(linkid) {
    return eleToNodeAddr.get(eleidReverseMap.get(linkid).parent);
}

function link(eleid1, eleid2) {
    let t1 = eleidReverseMap.get(eleid1);
    let t2 =  eleidReverseMap.get(eleid2);
    
    if (t1.type == t2.type) return;

   clock_link(t1, t2);



    console.log(t1);
    console.log(t2);
}

function clock_link(t1, k) {
    if (t1.type == "input") {
        if (t1.con != null) {
            let t = eleidReverseMap.get(t1.con);
            if (t.type != "output") {
                console.log("Something is fucked up... Sad wtf...");
            }
            t.con = exclude_rebuild_list(t.con, t1.eleid);
            console.log(t);
        }

        if (k.eleid == t1.con) {
            t1.con = null;
        } else {
            t1.con = k.eleid;
            k.con.push(t1.eleid);
        }
    } else if (t1.type == "output") {
        if (k.type != "input") return;

        let flag0 = false;
        t1.con.forEach(v => {
            if (v==k.eleid) flag0 = true;
        });

        if (flag0) {
            t1.con = exclude_rebuild_list(t1.con, k.eleid);
            k.con = null;
        } else {
            t1.con.push(k.eleid);
            k.con = t1.eleid;
        }
    }
}

function save_prefs_for(ele, part_l) {
    part_l.forEach((part) => {
        if (part[1] == "text") {
            ele.prefs[part[2]].val = document.getElementById(part[0]).value;
        }
    });
}

function gen_prefs_window_of_node(ele) {
    let gen = "";
    let part_l = [];
    if (ele.ninf.prefs != undefined) {
        Object.values(ele.prefs).forEach((pref) => {
            // TODO: Place for plugin adjustement
            let sub_id = crypto.randomUUID();
            if (pref.type == "text" || pref.type == "int" || pref.type == "float") {
                part_l.push([sub_id, pref.type, pref.id]);
                let part = `<div><label>${pref.name} </label><input id="${sub_id}" value="${pref.val}"></div>\n`
                gen+=part;
            }
        });
    }
    let button_uuid = crypto.randomUUID();
    let save_button = `<button id="${button_uuid}" style="position: absolute; right: 5px; bottom: 5px;">Save</button>\n`;
    gen+=save_button;
    create_dwindow(`${ele.ninf.name}'s preference`, new_dim(600, 400), gen);
    document.getElementById(button_uuid).onclick = () => save_prefs_for(ele, part_l);
}

function add_node_to_prj(node) {
    node.prefs = {};
    node.ninf.prefs?.forEach((pref) => node.prefs[pref.id] = {...pref});
    PROJECT.nodes.push(node);
    eleToNodeAddr.set(node.uuid, node);

    //
    let root = document.createElement("div");
    root.id = node.uuid;
    root.className = "dnode";

    let title = document.createElement("div");
    title.innerHTML = node.ninf.name;
    title.className = "dnode-title";
    root.appendChild(title);

    for (let i = 0; i < node.ninf.format.length; i++) {
        let form = node.ninf.format[i];

        let block = document.createElement("div");
        block.className = "dblock";
        if (form.block == "header") {
            block.innerHTML = `<p class="dnode-header"><b>${form.text}</b></p>`;
        } else if (form.block == "input" || form.block == "output") {
            let tuid = crypto.randomUUID();

            block.innerHTML = `<p class="dnode-text">${form.text}<span class="dnode-ck-${form.block}" id="${tuid}" style="background-color: #${TYPELESS.get(form.type).color}"></span></p>`;
            var lk = {
                parent: node.uuid,
                unid: form.unid,
                type: form.block,
                dtype: form.type,
                eleid: tuid,
                con: form.block == "output" ? [] : null
            };
            node.links.push(lk);
            eleidReverseMap.set(lk.eleid, lk);
        }
        
        root.setAttribute("mstate", "false");
        root.onmousedown = function (event) {root.setAttribute("mstate", "true")};
        root.onmouseup = function (event) {root.setAttribute("mstate", "false")};
        


        root.appendChild(block);
    }


    root.style = `left: ${actual_m_pos[0]}px; top: ${actual_m_pos[1]}px; width: ${node.ninf.width}px;`;

    document.getElementById("wsp").appendChild(root);

    for (let i = 0; i < node.links.length; i++) {
        let v = node.links[i];
        document.getElementById(v.eleid).onmousedown = function(event) {
            if (SELECTED_LINK == null) {
                SELECTED_LINK = v;
            } else {
                if (SELECTED_LINK == v) {
                    SELECTED_LINK = null;
                } else {
                    link(SELECTED_LINK.eleid, v.eleid);
                    SELECTED_LINK = null;
                }
            }
        }
    }

    //
}

let lr_config = {
    name: "Test project 1",
    author: "qigan",
    using: [
        "defpack"
    ],
    nodes: [],
    ext_prefs: {}
}

function load_project(prj) {
    request_packs().then((a) => {
        let tz = new Map();
        a.forEach(c => tz.set(c.id, c));

        let flag0 = true;
        prj.using.forEach(v => {
            if (!tz.has(v)) flag0 = false;
        });

        if (flag0) {

                PROJECT = prj;
                
                let node_list = [];
                let type_list = [];
                tz.forEach((v, k) => {
                    v.nodes.forEach(c => {
                        c.id = `${k}:${c.id}`
                        node_list.push(c)}
                    );
                    v.types.forEach(c => type_list.push(c));
                });
                
                main_context_menu[0].val[0].val = [];

                node_list.forEach(vk => {
                    NODLESS[vk.id] = vk;
                    main_context_menu[0].val[0].val.push({
                        title: vk.name,
                        action: "fun",
                        val: function() {
                            add_node_to_prj(create_new_node(vk.id));
                        }
                    });
                });

                type_list.forEach(vk => {
                    TYPELESS.set(vk.name, vk);
                });



                
        } else {
            console.log("Not all packs are present... Failed to load project!") // TODO: Make better messaging
        }





    })
}