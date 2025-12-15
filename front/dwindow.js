/*
    Context menu format,
    Sub list always disappearing on mouse leave
*/
let test_context_meny = [
    {
        title: "File",
        action: "fun",
        val: function() {
            console.log("TEST VE");
        }
    },
    {
        title: "Dropd",
        action: "sub",
        w: 100,
        val: [
            {
                title: "Bitch",
                action: "fun",
                val: function() {}
            },
            {
        title: "File",
        action: "fun",
        val: function() {
            console.log("TEST VE");
        }
    },
    {
        title: "Dropd",
        action: "sub",
        w: 100,
        val: [
            {
                title: "Bitch",
                action: "fun",
                val: function() {}
            },
        ]
    }
        ]
    }
]


 function positionate(el) {
     var rect=el.getBoundingClientRect();
     return [rect.left, rect.top, rect.width, rect.height];
 }



var DWINDOWS = {}

function create_dwindow(title, dimensions, bodyHtml, specify_container) {
    let root = document.createElement("div");
    root.setAttribute("mstate", "false")
    root.className = "dwindow";

    let conts = document.getElementsByClassName("dwindow-container");

    let container = document.getElementById("container");

    let xc = dimensions.x == undefined ? container.offsetWidth/2-dimensions.width/2 : dimensions.x;
    let yc = dimensions.y == undefined ? container.offsetHeight/2-dimensions.height/2 : dimensions.y;
    dimensions.x = xc;
    dimensions.y = yc;


    let tem = document.createElement("div");
    tem.innerText = title;
    tem.className = "dwindow-title";
    tem.onmousedown = function (event) {root.setAttribute("mstate", "true")};
    tem.onmouseup = function (event) {root.setAttribute("mstate", "false")};
    root.appendChild(tem);

    let body_part = document.createElement("div");
    body_part.innerHTML = bodyHtml;
    body_part.className = "dwindow-body";
    root.appendChild(body_part);
    document.addEventListener('keydown', function (event) {
        if (event.code == "Escape" /*&& is_mouse_in([dimensions.x, dimensions.y, dimensions.width, dimensions.height])*/) root.remove();
    });

    let ids_of = []
    for (let i = 0; i < conts.length; i++) {
        if (specify_container != undefined && specify_container == conts[i].id || specify_container == undefined) {
            root.style = `left: ${xc}px; top: ${yc}px; width: ${dimensions.width}px; height: ${dimensions.height}px; z-index: 50;`;
            root.id = `${crypto.randomUUID()}`;
            conts[i].appendChild(root);
            ids_of.push(root.id);
        }
    }

    return ids_of;
}

function is_mouse_in(v) {
    //console.log(`${actual_m_pos[0]} ${actual_m_pos[1]} --- ${v[0]}+${v[2]} ${v[1]}+${v[3]}`);
    return (v[0] <= actual_m_pos[0] && actual_m_pos[0] <= v[0]+v[2] && v[1] <= actual_m_pos[1] && actual_m_pos[1] <= v[1]+v[3]);
}

function check___ex(areas) {
    for (let i = 0; i < areas.length; i++) {
        let v = areas[i];
        //console.log(`${v[0]} <= ${actual_m_pos[0]} <= ${v[0]+v[2]} && ${v[1]} <= ${actual_m_pos[1]} <= ${v[1]+v[3]}`)
        if (is_mouse_in(v)) return false;
    }
    return true;
}

function create_context_menu(x, y, width, menu, stable, areas) {
        console.log(x,y,width);
    if (areas == undefined) areas = [];
    let root = document.createElement("div");
    root.style = `width: ${width}px; left: ${x}px; top: ${y}px; z-index: 30;`;
    root.setAttribute("stable", stable);
    root.className = "dcontext";
    root.onmouseleave = function (event) {
        setTimeout(() => {
            areas.push(positionate(root));
            if (root.getAttribute("stable") == "false" && check___ex(areas)) {
                root.remove();
            }
        }, 100)
    }
    for (let i = 0; i < menu.length; i++) {
        let el = menu[i];
        let part = document.createElement("div");
        part.className = "dcontext-part";
        part.textContent = el.title;
        if (el.action == "fun") {
            part.onclick = function (event) {
                root.remove();
                el.val();
            }
        } else if (el.action == "sub") {
            let transf = 0;
            part.onmouseenter = function (event) {
                let p = positionate(part);
                transf = create_context_menu(x+width, p[1], el.w, el.val, "false", [p]);
                root.setAttribute("stable", "true");
            }

            part.onmouseleave = function (event) {
                root.setAttribute("stable", stable);
                transf.onmouseleave();
                //transf.remove();
            }
        }
        root.appendChild(part);
    }
    document.getElementById("cm-container").appendChild(root);
    return root;
}

function new_dim(w, h) {
    return {width: w, height: h}
}

function new_full_dim(x, y, w, h) {
    return {x: x, y: y, width: w, height: h}
}

