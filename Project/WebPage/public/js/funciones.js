const listaContactos = document.querySelector('#contact');
eventListeners();

function eventListeners() {
     // Listener para eliminar el boton
     if(listaContactos) {
          listaContactos.addEventListener('click', SeleccionarContacto);
     }
}

function SeleccionarContacto(e) {
  //  const id = e.target.parentElement.getAttribute('data-id');
   //console.log(e.target.getAttribute('id'));
   //console.log(e.target.innerText);
   nombre = e.target.innerText;
   //console.log(e.target.parentElement.parentElement.parentElement.parentElement);
   //console.log(nombre);
   const respuesta = confirm('Are you sure?');

   if(respuesta) {
     //console.log(e.target.parentElement.parentElement.parentElement);
     e.target.parentElement.parentElement.parentElement.parentElement.innerHTML= "<div style='text-align : center; font-size : 50px; '><i class='fas fa-user'></i>&nbsp;&nbsp;" + nombre +"</div>";
    // console.log(document.getElementById("hide1"));
     document.getElementById("hide1").style.display = "block";
     document.getElementById("usernamefield").value = nombre;
   }
}


/*
document.getElementById("contactos").addEventListener("click", SeleccionarContacto);

function displayDate() {
  document.getElementById("demo").innerHTML = Date();
}

function SeleccionarContacto(e) {
     console.log("HOLA!");
}*/

/*
const demoId = document.getElementById('contact');
console.log(demoId);
*/
