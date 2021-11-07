(function connect() {
  let socket = io.connect();

  function fill_table(data) {
    data.forEach((element, ind) => {
      console.log(element);
      $("#myTable").append(`<tr id=${"tr_" + ind}>
        <td>${element[0]}</td>
        <td>${element[1]}</td>
        <td>${element[2]}</td>
        <td>${element[3]}</td>
        <td class="hovertext" data-hover="Read at: ${element[4][0]}">${element[4][1]}</td>
        <td class="hovertext" data-hover="Read at: ${element[5][0]}">${element[5][1]}</td>
        <td class="hovertext" data-hover="Read at: ${element[6][0]}">${element[6][1]}</td>
        <td>
			<select id="status">
				<option value="Urgent">Urgent</option>
				<option value="Stable">Stable</option>
			</select>
		</td>
        </tr>`);
		$("#myTable").find(`#tr_${ind}`).find("#status").val(`${element[7] ? "Urgent" : "Stable"}`);
    });
  }

  console.log("abcd")
  socket.emit("fetch_data", fill_table);

  socket.on("update_data", (patientInd, newData) => {
    $("#myTable").find(`#tr_${patientInd}`).find("td").eq(4).html(newData[4][1]);
    $("#myTable").find(`#tr_${patientInd}`).find("td").eq(5).html(newData[5][1]);
    $("#myTable").find(`#tr_${patientInd}`).find("td").eq(6).html(newData[6][1]);
    $("#myTable").find(`#tr_${patientInd}`).find("#status").val(`${newData[7] ? "Urgent" : "Stable"}`);
  });
})();
