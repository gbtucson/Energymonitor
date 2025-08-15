document.addEventListener("DOMContentLoaded", () => {
  const scheduleSelector = document.getElementById("scheduleSelector");
  const scheduleForm = document.getElementById("scheduleForm");
  const status = document.getElementById("status");
  const deleteBtn = document.getElementById("deleteScheduleBtn");

  let schedules = [];

  async function loadSchedules() {
    try {
      const res = await fetch("/schedule.json");
      if (!res.ok) throw new Error("Failed to load schedule.json");
      schedules = await res.json();

      scheduleSelector.innerHTML = "";
      schedules.forEach((sched, index) => {
        const option = document.createElement("option");
        option.value = index;
        option.textContent = sched.name;
        scheduleSelector.appendChild(option);
      });

      if (schedules.length > 0) {
        scheduleSelector.value = 0;
        populateForm(schedules[0]);
      }
      status.textContent = "";
    } catch (error) {
      console.error("Error loading schedules:", error);
      status.textContent = "Failed to load schedules.";
    }
  }

  function populateForm(schedule) {
    scheduleForm.name.value = schedule.name || "";
    scheduleForm.startDate.value = schedule.startDate || "";
    scheduleForm.endDate.value = schedule.endDate || "";

    const sections = schedule.sections || [];
    scheduleForm.section1Start.value = sections[0]?.start || "";
    scheduleForm.section1End.value = sections[0]?.end || "";
    scheduleForm.section2Start.value = sections[1]?.start || "";
    scheduleForm.section2End.value = sections[1]?.end || "";
    scheduleForm.section3Start.value = sections[2]?.start || "";
    scheduleForm.section3End.value = sections[2]?.end || "";
  }

  scheduleForm.addEventListener("submit", async (e) => {
    e.preventDefault();

    const newSchedule = {
      name: scheduleForm.name.value,
      startDate: scheduleForm.startDate.value,
      endDate: scheduleForm.endDate.value,
      sections: [
        { start: scheduleForm.section1Start.value, end: scheduleForm.section1End.value },
        { start: scheduleForm.section2Start.value, end: scheduleForm.section2End.value },
        { start: scheduleForm.section3Start.value, end: scheduleForm.section3End.value }
      ]
    };

    try {
      await fetch("/add-schedule", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(newSchedule)
      });
      status.textContent = "Schedule saved.";
      await loadSchedules();
    } catch (err) {
      console.error("Failed to save schedule:", err);
      status.textContent = "Failed to save schedule.";
    }
  });

  deleteBtn.addEventListener("click", async () => {
    const selectedIndex = scheduleSelector.value;
    if (selectedIndex === "") return;

    const scheduleToDelete = schedules[selectedIndex];
    if (!scheduleToDelete || !scheduleToDelete.name) return;

    try {
      await fetch("/delete-schedule", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ name: scheduleToDelete.name })
      });
      status.textContent = "Schedule deleted.";
      await loadSchedules();
    } catch (err) {
      console.error("Failed to delete schedule:", err);
      status.textContent = "Failed to delete schedule.";
    }
  });

  scheduleSelector.addEventListener("change", () => {
    const selectedIndex = scheduleSelector.value;
    if (selectedIndex !== "") {
      populateForm(schedules[selectedIndex]);
    }
  });

  loadSchedules();
});
