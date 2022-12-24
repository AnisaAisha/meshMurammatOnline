import * as THREE from 'https://cdn.skypack.dev/three@0.135.0/build/three.module.js'
import { OBJLoader } from 'https://cdn.skypack.dev/three@0.135.0/examples/jsm/loaders/OBJLoader.js'
import { PLYLoader } from 'https://cdn.skypack.dev/three@0.135.0/examples/jsm/loaders/PLYLoader.js'
import { OBJExporter } from 'https://cdn.skypack.dev/three@0.135.0/examples/js/exporters/OBJExporter.js'
import { PLYExporter } from 'https://cdn.skypack.dev/three@0.135.0/examples/js/exporters/PLYExporter.js'
import { OrbitControls } from 'https://cdn.skypack.dev/three@0.135.0/examples/jsm/controls/OrbitControls.js'

$('#file-input').change(function() {
    $('#hole-input').val($(this).val().replace(/^.*\\/, "").replace("obj", "ply").replace("off", "ply"));
    $('#fill-input').val($(this).val().replace(/^.*\\/, "").replace("obj", "ply").replace("off", "ply"));
});

var clock = new THREE.Clock(false);
var filled_time;
var detect_time;
var render_time;

var display_mesh;
var point_light;
var ambientLight;
var vertex_count;

function clearScene() {
    while (scene.children.length > 0) {
        scene.remove(scene.children[0]);
        console.log(scene.children.length);
    }
    // Create a directional light to show off the object
    point_light = new THREE.PointLight(0xffffff, 1, 0);
    point_light.position.set(50, 50, 50);
    scene.add(point_light);
    ambientLight = new THREE.AmbientLight(0x404040);
    scene.add(ambientLight);
}

var renderer = null,
    scene = null,
    camera = null;
var input = document.getElementById('file-input');
var inputSubmit = document.getElementById('file-submit');
var currentfilename = ""
var valids = [".ply", '.obj']
var patterns = /\.[0-9a-z]+$/i;
var extension = ""

/*Get file input from HTML file upload button*/
input.addEventListener('change', function(event) {
    console.log('submit clicked')
    console.log(this.files)
    clock.start();
    var file = this.files[0]; //Get the input file
    var reader = new FileReader(); //Asynchronously read file content	

    /*Listener is used to wait until file is loaded*/
    reader.addEventListener('load', function(event) {
        var contents = event.target.result;
        currentfilename = file['name']
        extension = (currentfilename).match(patterns)[0] //extract extension
            //PLY file loading
        if (extension === valids[0]) {
            //console.log(extension)
            //console.log(valids[0])
            console.log('ply detected')

            var loader = new THREE.PLYLoader();
            var mesh = loader.parse(contents); //Parse the .ply file content to get mesh
            mesh.computeVertexNormals();
            //console.log(mesh);
            console.log(mesh.attributes.position.count)
            vertex_count = mesh.attributes.position.count;

            if (mesh.index == null) {
                var material = new THREE.PointsMaterial({
                    size: 0.001,
                    color: 0xFFFFFF
                });
                display_mesh = new THREE.Points(mesh, material)
                display_mesh.scale.multiplyScalar(6);
            } else {
                var material = new THREE.MeshStandardMaterial({
                    vertexcolors: THREE.FaceColors
                });
                display_mesh = new THREE.Mesh(mesh, material);
                display_mesh.position.x = 0;
                display_mesh.position.y = 0;
                display_mesh.position.z = 0;
                display_mesh.scale.multiplyScalar(6);

                display_mesh.castShadow = true;
                display_mesh.receiveShadow = true;
            }
        }

        //OBJ File loading
        if (extension === valids[1]) {
            console.log('obj detected')
            display_mesh = new THREE.OBJLoader().parse(contents); //Parse the .obj file content to get mesh
            console.log(display_mesh)
        }

        if (extension === ".off") {
            var loader = new THREE.PLYLoader();
            var new_filename = currentfilename.slice(0, -3) + "ply";
            console.log(new_filename);
            /*setTimeout(function() {
                loader.load('./../routes/' + new_filename, function(geometry) {
                    console.log('./../routes/' + new_filename);
                    geometry.computeVertexNormals();
                    var material = new THREE.MeshStandardMaterial({
                        vertexColors: THREE.FaceColors
                    });
                });
            }, 60000);*/

            loader.load('./../routes/' + new_filename, function(geometry) {
                console.log('./../routes/' + new_filename);
                geometry.computeVertexNormals();
                var material = new THREE.MeshStandardMaterial({
                    vertexColors: THREE.FaceColors
                });
                display_mesh = new THREE.Mesh(geometry, material);
                console.log(display_mesh)
                display_mesh.position.x = 0;
                display_mesh.position.y = 0;
                display_mesh.position.z = 0;
                console.log(display_mesh);
            });
            //display_mesh = new THREE.Mesh(mesh, material);

            //console.log(display_mesh);
            /*var loader = new THREE.PLYLoader();
                    var mesh = loader.parse(contents); //Parse the .ply file content to get mesh
                    mesh.computeVertexNormals();
            
                    console.log(mesh.attributes.position.count)*/
            //vertex_count = mesh.attributes.position.count;4

            scene.add(display_mesh);
        }

        clearScene();

        inputSubmit.addEventListener('click', function(event) {
            if (display_mesh) {
                scene.add(display_mesh);
                $('#exampleModal').modal('hide');
            } else {
                console.log("An error occured!")
            }
        })

        render_time = clock.getDelta();
        clock.stop();
        console.log('render time: ', render_time);
        //console.log(scene);
        //console.log(display_mesh)
        //camera.position.set(display_mesh.position.x, display_mesh.position.x, 100);
        //camera.lookAt(display_mesh.position);

        console.log("file loaded")
        console.log(scene);
    }, false);

    /*Error check*/
    if (reader.readAsBinaryString !== undefined) {
        reader.readAsBinaryString(file);
    } else {
        reader.readAsArrayBuffer(file);
    }
});

function callHighlight() {
    clock.start();
    console.log(currentfilename)
    console.log("callHighlight called")
    setTimeout(function() {
        document.getElementById("displaymesh").click();
    }, 15000)
    console.log("callHighlight ended")
}

function callFilled() {
    clock.start();
    console.log("callFilled called")
    setTimeout(function() {
        document.getElementById("filledmesh").click();
    }, 10000)
    console.log("callFilled ended")
}

function loadHoleHighlight() {
    clearScene();
    var loader = new THREE.PLYLoader();

    loader.load('./../routes/detected_hole.ply', function(geometry) {
        console.log("ply loaded");
        geometry.computeVertexNormals();
        var material = new THREE.MeshStandardMaterial({
            vertexColors: THREE.FaceColors
        })

        display_mesh = new THREE.Mesh(geometry, material);

        display_mesh.position.x = 0;
        display_mesh.position.y = 0;
        display_mesh.position.z = 0;
        display_mesh.scale.multiplyScalar(10);

        display_mesh.castShadow = true;
        display_mesh.receiveShadow = true;
        scene.add(display_mesh);
        detect_time = clock.getDelta();
        clock.stop();
        console.log('detect time: ', detect_time);
        camera.position.set(display_mesh.position.x, display_mesh.position.x, 100);
        camera.lookAt(display_mesh.position);

    })

}



function loadWireframe() {
    scene.traverse(function(child) {
        if (child.isMesh) {
            child.material.wireframe = true;
        }
        display_mesh = child;
    });
    display_mesh.geometry.uvsNeedUpdate = true;
    display_mesh.needsUpdate = true;
}

function loadSolid() {
    display_mesh.traverse(function(child) {
        if (child.isMesh) {
            display_mesh.material.wireframe = false;
        }
    });
    display_mesh.geometry.uvsNeedUpdate = true;
    display_mesh.needsUpdate = true;
}

function loadFilledMesh() {
    clearScene();
    var loader = new THREE.PLYLoader();

    loader.load('./../routes/holefill.ply', function(geometry) {
        console.log("ply loaded");
        geometry.computeVertexNormals();
        var material = new THREE.MeshStandardMaterial({
            color: 0xd3d3d3
        });

        display_mesh = new THREE.Mesh(geometry, material);

        display_mesh.position.x = 0;
        display_mesh.position.y = 0;
        display_mesh.position.z = 0;
        display_mesh.scale.multiplyScalar(10);

        display_mesh.castShadow = true;
        display_mesh.receiveShadow = true;
        scene.add(display_mesh);
        filled_time = clock.getDelta();
        clock.stop();
        console.log('filled time: ', filled_time);
        camera.position.set(display_mesh.position.x, display_mesh.position.x, 100);
        camera.lookAt(display_mesh.position);

    })

}

function onLoad() {
    // Grab our container div
    var container = document.getElementById("container");

    // Create the Three.js renderer, add it to our div
    renderer = new THREE.WebGLRenderer({
        antialias: true
    });
    renderer.setSize(container.offsetWidth, container.offsetHeight);
    container.appendChild(renderer.domElement);

    // Create a new Three.js scene
    scene = new THREE.Scene();
    // Put in a camera
    camera = new THREE.PerspectiveCamera(45, container.offsetWidth / container.offsetHeight, 0.01, 10000);
    camera.position.set(0, 10, 10);

    // Create a directional light to show off the object
    point_light = new THREE.PointLight(0xffffff, 1, 0);
    ambientLight = new THREE.AmbientLight(0x404040);
    point_light.position.set(50, 50, 100);
    scene.add(point_light);
    scene.add(ambientLight);

    //Add controls to zoom in and out
    var controls = new THREE.OrbitControls(camera, renderer.domElement);
    controls.zoomSpeed = 0.4;
    controls.panSpeed = 0.4;

    // Run our render loop
    render();


}

function render() {
    requestAnimationFrame(render);
    renderer.render(scene, camera);
};

var saveData = (function() {
    var a = document.createElement("a");
    document.body.appendChild(a);
    a.style = "display: none";
    return function(data, fileName) {
        var blob = new Blob([data], {
                type: "octet/stream"
            }),
            url = window.URL.createObjectURL(blob);
        a.href = url;
        a.download = fileName;
        a.click();
        window.URL.revokeObjectURL(url);
    };
}());

function exportPLY(mesh) {
    // Instantiate an exporter
    var exporter = new THREE.PLYExporter();
    console.log(mesh);
    // Parse the input and generate the ply output
    const data = exporter.parse(mesh);
    //downloadFile(data);
    saveData(data, 'ply-output.ply')
    sendPLYFile(data)
        //return data;
}

function sendPLYFile(filedata) {
    $.ajax({
        type: "POST",
        url: "http://localhost:3000/upload",
        data: {
            filedata
        }
    }).done(function(msg) {
        alert("Data Saved: " + msg);
    });
}

function exportOBJ(mesh) {
    // Instantiate an exporter
    var exporter = new THREE.OBJExporter();
    // Parse the input and generate the obj output
    const data = exporter.parse(mesh);
    saveData(data, 'obj-output.obj')
}

function exportAs(mesh, filetype) {
    //let m = scene.children.find(obj => obj.type === 'Mesh')
    if (filetype === "obj") {
        exportOBJ(mesh);
    } else if (filetype === "ply") {
        exportPLY(mesh);
    }

}

function openExportForm() {
    document.getElementById("export").style.display = "block";
}

function closeExportForm() {
    document.getElementById("export").style.display = "none";
}


function basicMaterial() {
    basic_color = document.getElementById("basiccolor").value;
    scene.traverse(function(child) {
        if (child.isMesh) {
            child.material = new THREE.MeshBasicMaterial({
                color: basic_color
            });
        }
        display_mesh = child;
    });
    display_mesh.geometry.uvsNeedUpdate = true;
    display_mesh.needsUpdate = true;
}

function lambertMaterial() {
    scene.traverse(function(child) {
        var lambert_color = document.getElementById("lambertcolor").value;
        var lambert_emissive = document.getElementById("lambertemissivecolor").value;
        if (child.isMesh) {
            child.material = new THREE.MeshLambertMaterial({
                color: lambert_color,
                reflectivity: 10,
                refractionRatio: 0.98,
                emissive: lambert_emissive
            });
        }
        display_mesh = child;
    });
    display_mesh.geometry.uvsNeedUpdate = true;
    display_mesh.needsUpdate = true;
}

function phongMaterial() {
    var phong_color = document.getElementById("phongcolor").value;
    var phong_emissive = document.getElementById("phongemissivecolor").value;
    var phong_specular = document.getElementById("phongspecularcolor").value;
    var phong_shininess = document.getElementById("phongshininess").value;
    var phong_flatshading = document.getElementById("phongflatshading").checked;
    scene.traverse(function(child) {
        if (child.isMesh) {
            child.material = new THREE.MeshPhongMaterial({
                color: phong_color,
                emissive: phong_emissive,
                specular: phong_specular,
                shininess: phong_shininess,
                flatShading: phong_flatshading,
                reflectivity: 1,
                refractionRatio: 0.98,
            });
        }
        display_mesh = child;
    });
    display_mesh.geometry.uvsNeedUpdate = true;
    display_mesh.needsUpdate = true;
}


/*var slider = document.getElementById("myRange");
var output = document.getElementById("demo");
output.innerHTML = slider.value; // Display the default slider value

var slider2 = document.getElementById("phongshininess");
var shininess_output = document.getElementById("demo_shininess");
shininess_output.innerHTML = slider2.value; // Display the default slider value

// Update the current slider value (each time you drag the slider handle)
slider.oninput = function() {
    output.innerHTML = this.value;
}

slider2.oninput = function() {
    shininess_output.innerHTML = this.value;
}*/

function update_lightpos(val, x, y, z) {
    //console.log(x, y, z)
    //console.log(val)
    point_light.position.set(x, y, z);
    point_light.intensity = val;
}

function update_ambient() {
    //console.log(document.getElementById("ambientcolor").value)
    //console.log(scene)
    ambientLight.color = document.getElementById("ambientcolor").value;
}

function displayAxis() {
    const axesHelper = new THREE.AxesHelper(10);
    scene.add(axesHelper);
}

function toggleFullScreen() {
    let fullScreenText = document.getElementById('fullscreen');
    let elem = document.body;
    if (!document.fullscreenElement) {
        fullScreenText.innerHTML = 'Exit Fullscreen'
        elem.requestFullscreen().catch((err) => {
            console.log(`Error attempting to enable fullscreen mode: ${err.message} (${err.name})`);
        });
    } else {
        fullScreenText.innerHTML = 'Open in Fullscreen'
        document.exitFullscreen();
    }
}

function openStats() {
    //document.getElementById("statistics").style.display = "block";
    document.getElementById("num-faces").innerHTML = "No. of faces: " + renderer.info.render.faces;
    document.getElementById("num-vertices").innerHTML = "No. of vertices: " + vertex_count;
    console.log(renderer.info);

}