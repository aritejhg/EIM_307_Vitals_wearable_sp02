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
        <td>${element[5]}</td>
        <td>${element[6]}</td>
        <td>${element[7]}</td>
        </tr>`);
    });
  }

  socket.emit("fetch_data", fill_table);

  socket.on("update_data", (patientInd, newData) => {
    $("#myTable").find(`#tr_${patientInd}`).find("td").eq(4).html(newData[4]);
    $("#myTable").find(`#tr_${patientInd}`).find("td").eq(5).html(newData[5]);
    $("#myTable").find(`#tr_${patientInd}`).find("td").eq(6).html(newData[6]);
  });
})();
