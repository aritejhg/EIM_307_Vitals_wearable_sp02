(function connect() {
  let socket = io.connect();

  function fill_table(data) {
    data.forEach((element, ind) => {
      $("#myTable").append(`<tr id=${"tr_" + ind}>
        <td>${element[0]}</td>
        <td>${element[1]}</td>
        <td>${element[2]}</td>
        <td>${element[3]}</td>
        <td>${element[4]}</td>
        </tr>`);
    });
  }

  socket.emit("fetch_data", fill_table);

  socket.on("update_data", (to_change) => {
    ind = to_change[0];
    $("#myTable").find();
  });
})();
