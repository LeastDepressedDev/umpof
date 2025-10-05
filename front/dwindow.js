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

var actual_m_pos = [0, 0]
var deltas = [0, 0]

document.body.onmousemove = function (event) {
    deltas = [event.clientX - actual_m_pos[0], event.clientY - actual_m_pos[1]]
    actual_m_pos = [event.clientX, event.clientY]
}

document.body.onmouseleave = function (event) {
    let eles = document.getElementsByClassName("dwindow");
    for (let i = 0; i < eles.length; i++) {
        eles[i].setAttribute("mstate", "false");
    }
}

function create_dwindow(title, dimensions, bodyHtml, specify_container) {
    let root = document.createElement("div");
    root.setAttribute("mstate", "false")
    root.className = "dwindow";

    let conts = document.getElementsByClassName("dwindow-container");

    let container = document.getElementById("container");

    let xc = dimensions.x == undefined ? container.offsetWidth/2-dimensions.width/2 : dimensions.x;
    let yc = dimensions.y == undefined ? container.offsetHeight/2-dimensions.height/2 : dimensions.y;


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

    for (let i = 0; i < conts.length; i++) {
        if (specify_container != undefined && specify_container == conts[i].id || specify_container == undefined) {
            root.style = `left: ${xc}px; top: ${yc}px; width: ${dimensions.width}px; height: ${dimensions.height}px;`
            conts[i].appendChild(root);
        }
    }
}

document.body.onclick = function (event) {
    let eles = document.getElementsByClassName("dcontext");
    for (let i = 0; i < eles.length; i++) {
        eles[i].remove();
    }
}

function is_mouse_in(v) {
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
    if (areas == undefined) areas = [];
    let root = document.createElement("div");
    root.style = `width: ${width}px; left: ${x}px; top: ${y}px;`
    root.setAttribute("stable", stable)
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


/*
    This is probably unnecessary but who cares
*/
setInterval(() => {
    let eles = document.getElementsByClassName("dcontext");
    for (let i = 0; i < eles.length; i++) {
        if (eles[i].getAttribute("stable") == "false") {
            if (check___ex(dcontext_mapper[eles[i]])) {
                eles[i].remove();
            }
        }
    }
}, 10);





setInterval(() => {
    let eles = document.getElementsByClassName("dwindow");
    for (let i = 0; i < eles.length; i++) {
        if (eles[i].getAttribute("mstate") == "true") {
            //console.log(parseInt(eles[i].style.left) + deltas[0])
            eles[i].style.left = `${parseInt(eles[i].style.left) + deltas[0]}px`;
            eles[i].style.top = `${parseInt(eles[i].style.top) + deltas[1]}px`;
        }
    }
    deltas = [0, 0]
}, 1);

function new_dim(w, h) {
    return {width: w, height: h}
}

function new_full_dim(x, y, w, h) {
    return {x: x, y: y, width: w, height: h}
}

