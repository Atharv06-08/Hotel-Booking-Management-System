const STORAGE_KEY = "aurora-stay-state-v1";

const defaultRooms = [
  ...[101, 102, 103, 104, 105, 106, 107, 108].map((n) => ({ roomNumber: n, roomType: "Standard", pricePerNight: 2200, occupied: false })),
  ...[201, 202, 203, 204, 205, 206].map((n) => ({ roomNumber: n, roomType: "Deluxe", pricePerNight: 3500, occupied: false })),
  ...[301, 302, 303, 304].map((n) => ({ roomNumber: n, roomType: "Suite", pricePerNight: 6000, occupied: false }))
];

const seedState = {
  rooms: defaultRooms,
  customers: [],
  bookings: [],
  nextCustomerId: 1,
  nextBookingId: 1,
  feed: ["System ready. Front desk online."]
};

const state = loadState();

function loadState() {
  try {
    const raw = localStorage.getItem(STORAGE_KEY);
    if (!raw) return structuredClone(seedState);
    const parsed = JSON.parse(raw);
    return { ...structuredClone(seedState), ...parsed };
  } catch {
    return structuredClone(seedState);
  }
}

function saveState() {
  localStorage.setItem(STORAGE_KEY, JSON.stringify(state));
}

function tierForStays(stays) {
  if (stays >= 6) return "Gold";
  if (stays >= 3) return "Silver";
  return "Normal";
}

function discountRateForStays(stays) {
  if (stays >= 6) return 0.1;
  if (stays >= 3) return 0.05;
  return 0;
}

function currency(amount) {
  return `Rs. ${amount.toFixed(2)}`;
}

function addFeed(text) {
  state.feed.unshift(`${new Date().toLocaleTimeString()} - ${text}`);
  state.feed = state.feed.slice(0, 30);
}

function renderStats() {
  const totalRooms = state.rooms.length;
  const available = state.rooms.filter((r) => !r.occupied).length;
  const occupied = totalRooms - available;
  const active = state.bookings.filter((b) => b.active).length;
  byId("statTotalRooms").textContent = totalRooms;
  byId("statAvailable").textContent = available;
  byId("statOccupied").textContent = occupied;
  byId("statActive").textContent = active;
}

function renderRooms() {
  const node = byId("roomsList");
  node.innerHTML = "";
  state.rooms.forEach((room) => {
    node.append(row(`
      <strong>Room ${room.roomNumber}</strong> - ${room.roomType}
      <span class="badge ${room.occupied ? "occupied" : "available"}">${room.occupied ? "Occupied" : "Available"}</span>
      <br><small>${currency(room.pricePerNight)} per night</small>
    `));
  });
}

function renderBookings() {
  const node = byId("bookingsList");
  node.innerHTML = "";
  const active = state.bookings.filter((b) => b.active);
  if (!active.length) {
    node.append(row("<small>No active bookings.</small>"));
    return;
  }
  active.forEach((b) => {
    const room = state.rooms.find((r) => r.roomNumber === b.roomNumber);
    node.append(row(`
      <strong>#${b.bookingId}</strong> - ${b.customerName}
      <br><small>Room ${b.roomNumber} (${room?.roomType || "Unknown"}) | Customer ID ${b.customerId}</small>
    `));
  });
}

function renderCustomers() {
  const node = byId("customersList");
  node.innerHTML = "";
  if (!state.customers.length) {
    node.append(row("<small>No customers yet.</small>"));
    return;
  }
  [...state.customers]
    .sort((a, b) => b.totalSpent - a.totalSpent)
    .forEach((c) => {
      const tier = tierForStays(c.stays);
      node.append(row(`
        <strong>${c.name}</strong>
        <span class="badge tier-${tier.toLowerCase()}">${tier}</span>
        <br><small>ID ${c.customerId} | ${c.contact} | ${c.stays} stays | ${currency(c.totalSpent)}</small>
      `));
    });
}

function renderFeed() {
  const node = byId("activityFeed");
  node.innerHTML = "";
  state.feed.forEach((item) => {
    const li = document.createElement("li");
    li.textContent = item;
    node.appendChild(li);
  });
}

function render() {
  renderStats();
  renderRooms();
  renderBookings();
  renderCustomers();
  renderFeed();
}

function byId(id) {
  return document.getElementById(id);
}

function row(html) {
  const div = document.createElement("div");
  div.className = "row";
  div.innerHTML = html.trim();
  return div;
}

function toast(message) {
  const node = byId("toast");
  node.textContent = message;
  node.classList.remove("hidden");
  setTimeout(() => node.classList.add("hidden"), 2200);
}

function checkInGuest(evt) {
  evt.preventDefault();
  const name = byId("guestName").value.trim();
  const contact = byId("guestContact").value.trim();
  const roomType = byId("roomType").value;
  if (!name || !contact || !roomType) return;

  const room = state.rooms.find((r) => r.roomType === roomType && !r.occupied);
  if (!room) {
    toast(`No ${roomType} rooms available.`);
    return;
  }

  let customer = state.customers.find((c) => c.contact === contact);
  if (!customer) {
    customer = { customerId: state.nextCustomerId++, name, contact, stays: 0, totalSpent: 0 };
    state.customers.push(customer);
    addFeed(`New customer profile created: ${name}`);
  }

  const booking = {
    bookingId: state.nextBookingId++,
    roomNumber: room.roomNumber,
    customerId: customer.customerId,
    customerName: customer.name,
    checkInTime: Date.now(),
    active: true
  };

  state.bookings.push(booking);
  room.occupied = true;
  addFeed(`Checked in ${customer.name} to room ${room.roomNumber}`);
  saveState();
  render();
  evt.target.reset();
  toast(`Check-in successful. Room ${room.roomNumber} assigned.`);
}

function checkOutGuest(evt) {
  evt.preventDefault();
  const roomNumber = Number(byId("checkoutRoomNumber").value);
  if (!roomNumber) return;

  const room = state.rooms.find((r) => r.roomNumber === roomNumber);
  if (!room || !room.occupied) {
    toast("Room not occupied or invalid.");
    return;
  }

  const booking = state.bookings.find((b) => b.roomNumber === roomNumber && b.active);
  if (!booking) {
    toast("No active booking found.");
    return;
  }

  const customer = state.customers.find((c) => c.customerId === booking.customerId);
  if (!customer) {
    toast("Customer data missing for booking.");
    return;
  }

  const nights = Math.max(1, Math.floor((Date.now() - booking.checkInTime) / (1000 * 60 * 60 * 24)));
  const baseBill = nights * room.pricePerNight;
  const futureStays = customer.stays + 1;
  const discountRate = discountRateForStays(futureStays);
  const discountAmount = baseBill * discountRate;
  const total = baseBill - discountAmount;

  customer.stays = futureStays;
  customer.totalSpent += total;
  room.occupied = false;
  booking.active = false;

  byId("billCard").classList.remove("hidden");
  byId("billCard").innerHTML = `
    <strong>Billing Summary</strong><br>
    Guest: ${customer.name}<br>
    Room: ${room.roomNumber} (${room.roomType})<br>
    Nights: ${nights}<br>
    Base: ${currency(baseBill)}<br>
    Discount: ${(discountRate * 100).toFixed(0)}% (${currency(discountAmount)})<br>
    <strong>Total: ${currency(total)}</strong>
  `;

  addFeed(`Checked out ${customer.name} from room ${room.roomNumber} | Bill ${currency(total)}`);
  saveState();
  render();
  evt.target.reset();
  toast("Check-out complete. Bill generated.");
}

function resetDemo() {
  Object.assign(state, structuredClone(seedState));
  saveState();
  byId("billCard").classList.add("hidden");
  addFeed("Demo data reset.");
  render();
  toast("Demo reset complete.");
}

byId("checkInForm").addEventListener("submit", checkInGuest);
byId("checkOutForm").addEventListener("submit", checkOutGuest);
byId("resetDemo").addEventListener("click", resetDemo);

render();
