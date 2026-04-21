/**
 * Database lab — queries.js
 * ---------------------
 * Standalone script: connect to the same DB, run example aggregate SELECTs.
 * Run after you have submitted at least one CV via the web form:
 *   node queries.js
 *   npm run queries
 */

const { connectDb, getPool } = require("./db");

async function runQueries() {
  await connectDb();
  const pool = getPool();

  // —— QUERY 1: COUNT — how many courses per person (LEFT JOIN keeps people with 0 courses)
  console.log("\n── QUERY 1: Number of courses per person ──");

  const [courseCounts] = await pool.query(`
    SELECT p.fName, p.lName, COUNT(c.idcourse) AS courseCount
    FROM person p
    LEFT JOIN course c ON c.person_idperson = p.idperson
    GROUP BY p.idperson
    ORDER BY courseCount DESC
  `);

  courseCounts.forEach((row) =>
    console.log(`  ${row.fName} ${row.lName} → ${row.courseCount} course(s)`),
  );

  // —— QUERY 2: only persons with more than 1 project
  console.log("\n── QUERY 2: Persons with more than 1 project ──");

  const [topPerson] = await pool.query(`
    SELECT p.fName, p.lName, COUNT(pr.idproject) AS projectCount
    FROM person p
    INNER JOIN project pr ON pr.person_idperson = p.idperson
    GROUP BY p.idperson
    HAVING projectCount > 1
    ORDER BY projectCount DESC
  `);

  if (topPerson.length > 0) {
    topPerson.forEach((t) =>
      console.log(`  ${t.fName} ${t.lName} — ${t.projectCount} project(s)`),
    );
  } else {
    console.log("  No data yet.");
  }

  // —— QUERY 3: DISTINCT — list unique countries in person table
  console.log("\n── QUERY 3: Unique countries ──");

  const [distinctCountries] = await pool.query(`
    SELECT DISTINCT country
    FROM person
    ORDER BY country ASC
  `);

  distinctCountries.forEach((row) => console.log(`  ${row.country || "N/A"}`));

  // —— QUERY 4: DELETE — remove persons with no city set
  // console.log('\n── QUERY 4: Delete persons with no city ──');

  // const [deleteResult] = await pool.query(`
  //   DELETE FROM person
  //   WHERE city IS NULL OR city = ''
  // `);

  // console.log(`  Deleted ${deleteResult.affectedRows} person(s) with no city.`);

  // —— QUERY 5: UPDATE — update email for person with id = 1
  console.log("\n── QUERY 5: Update email for person with id = 1 ──");

  const [updateResult] = await pool.query(`
  UPDATE person p SET p.email = 'test@updated.com' where p.idperson = 1 ;
`);
  console.log(`  Updated ${updateResult.affectedRows} person(s) email(s).`);

  // ======================================== TASK =============================================================
  // 1- Show persons who are enrolled in more than 2 courses, display their full name and course count
  console.log("\n── TASK 1 ──");

  const [task1] = await pool.query(`
    SELECT p.fName, p.lName, COUNT(c.idcourse) AS courseCount
    FROM person p
    JOIN course c ON c.person_idperson = p.idperson
    GROUP BY p.idperson
    HAVING courseCount > 2
    ORDER BY courseCount DESC
  `);

  task1.forEach((r) => console.log(`${r.fName} ${r.lName} → ${r.courseCount}`));

  // 2- list each distinct country and the number of persons in it, only show countries with more than 2 persons
  console.log("\n── TASK 2 ──");

  const [task2] = await pool.query(`
    SELECT country, COUNT(*) AS personCount
    FROM person
    GROUP BY country
    HAVING personCount > 2
  `);

  task2.forEach((r) => console.log(`${r.country} → ${r.personCount}`));

  // 3- Update the email of all persons who have at least one project, set it to their firstName + lastName + '@company.com'
  console.log("\n── TASK 3 ──");

  const [task3] = await pool.query(`
  UPDATE person p
  SET p.email = CONCAT(p.fName, p.lName, '@company.com')
  WHERE EXISTS (
    SELECT 1 FROM project pr
    WHERE pr.person_idperson = p.idperson
  )
`);

  console.log(`Updated: ${task3.affectedRows}`);

  // 4- Delete all courses that belong to persons from a specific country
  console.log("\n── TASK 4 ──");

  const country = "Egypt"; // تقدر تغيرها

  const [task4] = await pool.query(
    `
  DELETE c FROM course c
  JOIN person p ON c.person_idperson = p.idperson
  WHERE p.country = ?
`,
    [country],
  );

  console.log(`Deleted: ${task4.affectedRows}`);
  // 5- Show each country and the average number of languages spoken by persons from that country, only show countries where the average is more than 1
  console.log("\n── TASK 5 ──");

  const [task5] = await pool.query(`
    SELECT country, AVG(lang_count) AS avgLangs
    FROM (
        SELECT p.idperson, p.country, COUNT(l.name) AS lang_count
        FROM person p
        JOIN language l ON l.person_idperson = p.idperson
        GROUP BY p.idperson, p.country
    ) AS sub
    GROUP BY country
    HAVING avgLangs > 1
    ORDER BY avgLangs DESC;
  `);

  task5.forEach((row, idx) =>
    console.log(
      `${idx + 1}.  ${row.country} with an average of ${row.avgLangs} languages spoken per person`,
    ),
  );
  await pool.end();
}
runQueries().catch((err) => console.error("Error:", err.message));
