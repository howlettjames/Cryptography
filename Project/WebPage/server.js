const express = require('express');
const path = require('path');
const fs = require('fs');
const expressLayouts = require('express-ejs-layouts');
const mongoose = require('mongoose');
const flash = require('connect-flash');
const session = require('express-session');
const passport = require('passport');
const crypto = require('crypto');
const multer = require('multer');
const GridFsStorage = require('multer-gridfs-storage');
const Grid = require('gridfs-stream');
const methodOverride = require('method-override');
const usersModule = require('./routes/users.js');
const bcrypt = require('bcryptjs');
const toArray = require('stream-to-array');
const util = require('util');
const nodersa = require('node-rsa');

// User model
const User = require("./models/User");

const app = express();

// ------------------------------------------- LOGGER ---------------------------------------------
// const logger = (req, res, next) => 
// {
//     console.log(`${req.protocol}://${req.get('host')}${req.originalUrl}: ${moment().format()}`);
//     next();
// };

// // Init middleware
// app.use(logger);

// // Handlebars Middleware
// app.engine('handlebars', exphbs({defaultLayout: 'main'}));
// app.set('view engine', 'handlebars');
// ------------------------------------------- /LOGGER ---------------------------------------------

// Passport config
require('./config/passport')(passport);

// DB Config
const db = require('./config/keys').MongoURI;

// Connect to Mongo (using GridFS)
const conn = mongoose.createConnection(db, { useNewUrlParser: true });
    // .then(() => console.log('MongoDB Connected...'))
    // .catch(err => console.log(err));

// Connect to Mongo (standard way)
mongoose.connect(db, { useNewUrlParser: true })
.then(() => console.log('MongoDB Connected...'))
.catch(err => console.log(err));

// Init GFS
let gfs;
conn.once('open', () => 
{
    // Initialize stream
    gfs = Grid(conn.db, mongoose.mongo);
    gfs.collection('uploads');
});

// Create storage engine
const storage = new GridFsStorage
({
    url: db,
    file: (req, file) =>
    {
        let participants = [];
        for(let i = 0; i < req.body.noparticipants; i++)
        {
            switch(i)
            {
                case 0:
                    participants.push({ participantname: req.body.participant0, hasSigned: false });        
                break;
                case 1:
                    participants.push({ participantname: req.body.participant1, hasSigned: false });        
                break;
                case 2:
                    participants.push({ participantname: req.body.participant2, hasSigned: false });        
                break;
                case 3:
                    participants.push({ participantname: req.body.participant3, hasSigned: false });        
                break;
                case 4:
                    participants.push({ participantname: req.body.participant4, hasSigned: false });        
                break;
                case 5:
                    participants.push({ participantname: req.body.participant5, hasSigned: false });        
                break;
                case 6:
                    participants.push({ participantname: req.body.participant6, hasSigned: false });        
                break;
                case 7:
                    participants.push({ participantname: req.body.participant7, hasSigned: false });        
                break;
                case 8:
                    participants.push({ participantname: req.body.participant8, hasSigned: false });        
                break;
                case 9:
                    participants.push({ participantname: req.body.participant9, hasSigned: false });        
                break;
                case 10:
                    participants.push({ participantname: req.body.participant10, hasSigned: false });        
                break;
            }
        }
            
        const fileInfo = 
        {
            filename: file.originalname,
            bucketName: 'uploads',
            disableMD5: true,
            metadata: { participants: participants, ciphered: false  }
        };

        return fileInfo;
    }
});
const upload = multer({ storage });

// Create storage engine
const storageMemo = new GridFsStorage
({
    url: db,
    file: (req, file) =>
    {
        const fileInfo = 
        {
            filename: file.originalname,
            bucketName: 'uploadsMemo',
            disableMD5: true,
            metadata: { receiver: req.body.username, sender: req.body.sendername }
        };

        return fileInfo;
    }
});
const uploadMemo = multer({ storageMemo });

// Create storage engine for private keys uploads
const pkstorage = multer.memoryStorage();
const uploadpk = multer({ pkstorage });

// EJS
app.use(expressLayouts);
app.set('view engine', 'ejs');

// Body Parser Middleware
app.use(express.json());
app.use(express.urlencoded({ extended: false }));

// Method Override
app.use(methodOverride('_method'));

// Express Session
app.use(session({ 
    secret: 'secret',
    resave: true,
    saveUninitialized: true
}));

// Passport middleware
app.use(passport.initialize());
app.use(passport.session());

// Connect flash
app.use(flash());

// Global vars
app.use((req, res, next) => 
{
    res.locals.success_msg = req.flash('success_msg');
    res.locals.error_msg = req.flash('error_msg');
    res.locals.error = req.flash('error');
    res.locals.error_uploading_minute = req.flash('error_uploading_minute');
    next(); 
});

// Set static folder
app.use(express.static(path.join(__dirname, 'public')));

// @route POST /users/login
// @desc Handles Login Page
let currentUserType;
let currentUserEmail;
app.post('/users/login', (req, res, next) =>
{
    const { email, password } = req.body;
    
    currentUserEmail = email;

    // Match user
    User.findOne({ email: email })
    .then(user =>
    {
        if(!user)
        {
            req.flash('error_msg', 'That email is not registered');
            res.redirect('/users/login');
        }
        else
        {
            // Match password
            bcrypt.compare(password, user.password, (err, isMatch) =>
            {
                if(err) throw err;

                if(isMatch)
                {
                    currentUserType = user.type;
                    res.render('home.ejs', { typeUser: user.type});        
                }
                else
                {
                    req.flash('error_msg', 'Password incorrect');
                    res.redirect('/users/login');
                }
            });
        }
    })
    .catch(err => console.log(err));
   
    // OLD WAY
    /*
    passport.authenticate('local', 
    {
        successRedirect: '/users/home',
        failureRedirect: '/users/login',
        failureFlash: true
    })(req, res, next);
    */
});

// @route POST /users/upload
// @desc Uploads Minute to DB
app.post('/users/upload', upload.single('file'), (req, res) =>
{
    if(req.file.contentType === 'image/jpeg' || req.file.contentType === 'image/png')
    {
        req.flash('error_uploading_minute', 'The Minute must be in .doc or docx format');
        return res.render('home.ejs', { typeUser: currentUserType });
    }
    
    res.render('home.ejs', { typeUser: currentUserType, successMsg: 'Minute uploaded successfully!' });
});

// FOR USING WITH QUERIES THAT REQUIRE HOUR
// let dateOffset = (3 * 60 * 60 * 1000); // 3 hr
// let currentDate = new Date();
// currentDate.setTime(currentDate.getTime() - dateOffset);

// @route GET /users/signminute
// @desc Renders Minute EJS
app.get('/users/signminute', (req, res) => 
{
    gfs.collection('uploads');
    gfs.files.find({}).toArray((err, files) => 
    {
        // Check if files
        if(files) 
        {
            let minutes = [];
            User.findOne({ email: currentUserEmail })
            .then(user =>
            {
                if(user)
                {
                    let i, j;
                    for(i = 0; i < files.length; i++)
                    {
                        // Check if minute has not been ciphered
                        if(!files[i].metadata.ciphered)
                        {
                            for(j = 0; j < files[i].metadata.participants.length; j++)
                            {
                                if(user.username === files[i].metadata.participants[j].participantname)
                                    break;
                            }
                            // If true, this user wasn't in this Minute meeting
                            if(j === files[i].metadata.participants.length)
                            {
                                minutes.push({ filename: files[i].filename, available: false, hasSigned: false });
                            }
                            else // Else, this user was found on the list of participants
                            {
                                console.log(`${user.username} has signed ${files[i].filename} ? ${files[i].metadata.participants[j].hasSigned}`);
                                minutes.push({ filename: files[i].filename, available: true, hasSigned: files[i].metadata.participants[j].hasSigned });
                            }
                        }
                    }
                    res.render('signminute.ejs', { files: minutes });
                }
                else
                    res.render('signminute.ejs', { files: false, errorUserMsg: 'Error User Not Found'});
            });
        }
        else 
        {
            res.render('signminute.ejs', { files: false });
        }
    });
});

// @route POST /users/signminute
// @desc Receive Key from user
app.post('/users/signminute', uploadpk.single('keyfile'), (req, res) => 
{
    const { docname } = req.body;
    
    User.findOne({ email: currentUserEmail })
    .then(user =>
    {
        gfs.files.findOne({ filename: docname }, (err, fileInfo) =>
        {
            if(fileInfo)
            {
                let readstream = gfs.createReadStream
                ({
                    filename: fileInfo.filename,
                    root: "uploads"
                });

                toArray(readstream)
                .then(function (parts) 
                {
                    const buffers = parts
                    .map(part => util.isBuffer(part) ? part : Buffer.from(part));
                    let buffer = Buffer.concat(buffers);

                    // console.log('Field name: ' + req.file.fieldname);
                    // console.log('Original name: ' + req.file.originalname);
                    // console.log('Encoding: ' + req.file.encoding);
                    // console.log('Size: ' + req.file.size);
                    // console.log('File name: ' + req.file.filename);
                    // console.log('Buffer: ' + req.file.buffer);

                    console.log('User pair of keys are: \n' + req.file.buffer + '\n' + user.publickey);

                    // Paso 1 Emisor
                    const sign = crypto.createSign('SHA256');
                    sign.write(buffer);
                    sign.end();

                    // Paso 2 Emisor
                    const signature = sign.sign(req.file.buffer);

                    // Paso 1 Receptor
                    const verify = crypto.createVerify('SHA256');
                    verify.write(buffer);
                    verify.end();

                    // Paso 2 Receptor
                    if(verify.verify(user.publickey, signature))
                    {
                        // At this point we make a copy of the metadata of the original Minute to paste it into the now Ciphered Minute
                        // Find the participant and modify its "hasSigned" property to true
                        let participants = [];
                        let participantname;
                        let hasSigned;
                        for(i = 0; i < fileInfo.metadata.participants.length; i++)
                        {
                            if(user.username === fileInfo.metadata.participants[i].participantname)
                                fileInfo.metadata.participants[i].hasSigned = true;

                            participantname = fileInfo.metadata.participants[i].participantname;
                            hasSigned = fileInfo.metadata.participants[i].hasSigned;
                            participants.push({ participantname, hasSigned });
                        }
                        let metadataModified = { participants, ciphered: false };
                        gfs.files.updateOne({ filename: fileInfo.filename }, { $set: { metadata: metadataModified } });

                        req.flash('success_msg', 'You have signed up correctly!. And the system has verified your signature');
                        res.render('home.ejs', { typeUser: currentUserType, successMsg: 'You have signed up correctly!' });
                        // res.set('Content-Type', fileInfo.contentType);
                        // return readstream.pipe(res);
                    }
                    else
                    {
                        // req.flash('error_msg', 'The keys do not match, try again');
                        // res.redirect('/users/signminute');
                        res.render('signminute.ejs', { errorUserMsg: 'The keys do not match, try again', files: false });
                    }
                })
                .catch(err =>
                {
                    console.log(err);
                    req.flash('error_msg', 'Error: Problem ocurred trying to convert stream to buffer');
                    res.redirect('/users/signminute');
                });
                // let i;
                // Validate user who is signing is actually in Minute participants array
                // for(i = 0; i < fileInfo.metadata.participants.length; i++)
                // {
                //     if(user.username === fileInfo.metadata.participants[i].participantname)
                //     {
                //         console.log(`Username allowed is: ${user.username}`);
                //         break;    
                //     }
                // }

                // if(i === fileInfo.metadata.participants.length)
                //     res.render('home.ejs', { error_msg: 'You were not participant of that meeting', typeUser: currentUserType });
                // else
                // {
                //     let readstream = gfs.createReadStream
                //     ({
                //         filename: fileInfo.filename,
                //         root: "uploads"
                //     });

                //     toArray(readstream)
                //     .then(function (parts) 
                //     {
                //         const buffers = parts
                //         .map(part => util.isBuffer(part) ? part : Buffer.from(part));
                //         let buffer = Buffer.concat(buffers);

                //         console.log('User pair of keys are: \n' + key + '\n' + user.publickey);

                //         // Paso 1 Emisor
                //         const sign = crypto.createSign('SHA256');
                //         sign.write(buffer);
                //         sign.end();

                //         // Paso 2 Emisor
                //         const signature = sign.sign(key);

                //         // Paso 1 Receptor
                //         const verify = crypto.createVerify('SHA256');
                //         verify.write(buffer);
                //         verify.end();

                //         // Paso 2 Receptor
                //         if(verify.verify(user.publickey, signature))
                //         {
                //             // At this point we make a copy of the metadata of the original Minute to paste it into the now Ciphered Minute
                //             // Find the participant and modify its "hasSigned" property to true
                //             let participants = [];
                //             let participantname;
                //             let hasSigned;
                //             for(i = 0; i < fileInfo.metadata.participants.length; i++)
                //             {
                //                 if(user.username === fileInfo.metadata.participants[i].participantname)
                //                     fileInfo.metadata.participants[i].hasSigned = true;

                //                 participantname = fileInfo.metadata.participants[i].participantname;
                //                 hasSigned = fileInfo.metadata.participants[i].hasSigned;
                //                 participants.push({ participantname, hasSigned });
                //             }
                //             let metadataModified = { participants, ciphered: false };
                //             gfs.files.updateOne({ filename: fileInfo.filename }, { $set: { metadata: metadataModified } });

                //             req.flash('success_msg', 'You have signed up correctly!. And the system has verified your signature');
                //             res.render('home.ejs', { typeUser: currentUserType, successMsg: 'You have signed up correctly!' });
                //             // res.set('Content-Type', fileInfo.contentType);
                //             // return readstream.pipe(res);
                //         }
                //         else
                //         {
                //             // req.flash('error_msg', 'The keys do not match, try again');
                //             res.render('/users/signminute', { errorUserMsg: 'The keys do not match, try again' });
                //         }                
                //     })
                //     .catch(err =>
                //     {
                //         console.log(err);
                //         req.flash('error_msg', 'The keys do not match, try again');
                //         res.redirect('/users/signminute');
                //     });
                // }
            }
            else
                res.render('home.ejs', { error_msg: 'File not found', typeUser: currentUserType });
        });       
    });
});

// @route GET /users/cipherminute
// @desc Renders Cipher Minute EJS
app.get('/users/cipherminute', (req, res) => 
{
    gfs.files.find().toArray((err, files) => 
    {
        // Check if files
        if(files) 
        {
            let filesCompletelySigned = [];
            let filesUncompletelySigned = [];
            let i, j;
            let isCompletelySigned;
            for(i = 0; i < files.length; i++)
            {
                // Check if Minute is not ciphered
                console.log(`${files[i].filename} has been ciphered? ${files[i].metadata.ciphered}`);
                if(!files[i].metadata.ciphered)
                {
                    isCompletelySigned = true;
                    for(j = 0; j < files[i].metadata.participants.length; j++)
                    {
                        if(!files[i].metadata.participants[j].hasSigned)
                        {
                            isCompletelySigned = false;
                            break;
                        }
                    }
                    if(isCompletelySigned)
                        filesCompletelySigned.push(files[i]);
                    else
                        filesUncompletelySigned.push(files[i]);                    
                }
            }
            res.render('cipherminute.ejs', { filesCompletelySigned: filesCompletelySigned, filesUncompletelySigned: filesUncompletelySigned });
        }
        else 
            res.render('cipherminute.ejs', { filesCompletelySigned: false, filesUncompletelySigned: false });
    });
    // LAST VERSION
    // console.log(`Current date: ${currentDate}`);
    // gfs.files.findOne({ uploadDate: { $lt: new Date(), $gt: currentDate } }, (err, file) =>
});

// @route POST /users/cipherminute
// @desc Renders Cipher Minute EJS
app.post('/users/cipherminute', (req, res) => 
{
    const { password, docname } = req.body;

    gfs.files.findOne({ filename: docname }, (err, fileInfo) =>
    {
        if(fileInfo)
        {
            let readstream = gfs.createReadStream
            ({
                filename: fileInfo.filename,
                root: "uploads",
                mode: "r"
            });

            // Updating the original Minute field metadata.ciphered to true
            let metadataModified = { participants: fileInfo.metadata.participants, ciphered: true };
            gfs.files.updateOne({ filename: fileInfo.filename }, { $set: { metadata: metadataModified } });

            // Modify metadata.ciphered to true. Useless since it is already being stored into uploadsCiphered table
            fileInfo.metadata.ciphered = true;
            let writestream = gfs.createWriteStream
            ({
                filename: fileInfo.filename + '.aes',
                root: "uploadsCiphered",
                mode: "w",
                metadata: fileInfo.metadata
            });

            // const key = crypto.scryptSync(password, 'salt', 24);
            // const iv = crypto.randomBytes(16);
            const cipher = crypto.createCipher('aes-192-cbc', password);

            readstream.pipe(cipher).pipe(writestream);

            res.render('home.ejs', { typeUser: currentUserType, successMsg: 'Minute ciphered correctly!' });    
        }
        else
        {
            console.log('There is no file to cipher');
        }
    });
});

// @route GET /users/showminutes
// @desc Renders Show Minutes EJS
app.get('/users/showminutes', (req, res) => 
{
    const files = [];
    let i;

    gfs.collection('uploads');
    gfs.files.find().toArray((err, filesUnciphered) => 
    {
        gfs.collection('uploadsCiphered');
        gfs.files.find().toArray((err, filesCiphered) => 
        {   
            for(i = 0; i < filesUnciphered.length; i++)
                files.push(filesUnciphered[i]);

            for(i = 0; i < filesCiphered.length; i++)
                files.push(filesCiphered[i]);
                
            // Check if files
            if (!files || files.length === 0) 
                res.render('showminutes.ejs', { files: false });
            else 
                res.render('showminutes.ejs', { files: files });
        });
    });
});

// @route POST /users/download
// @desc Serves the file to be downloaded
app.post('/users/download', (req, res) =>
{
    const { docname, password } = req.body;

    const extension = path.extname(docname);
    if(extension.localeCompare('.aes') === 0)
    {
        gfs.collection('uploadsCiphered');
        gfs.files.findOne({ filename: docname }, (err, fileInfo) =>
        {
            if(err) throw err;

            if(fileInfo)
            {
                User.findOne({ email: currentUserEmail })
                .then(user =>
                {
                    if(user)
                    {
                        // Match password
                        bcrypt.compare(password, user.password, (err, isMatch) =>
                        {
                            if(err) throw err;

                            if(isMatch)
                            {
                                let readstream = gfs.createReadStream
                                ({
                                    filename: fileInfo.filename,
                                    root: "uploadsCiphered",
                                    mode: "r"
                                });
                                
                                res.set('Content-Type', 'text/plain');

                                let response = readstream.pipe(res);

                                res.on('finish', () => {return response});
                            }
                            else
                            {
                                req.flash('error_msg', 'Error: User password incorrect');
                                res.redirect('/users/showminutes');
                            }
                        });
                    }
                    else
                    {
                        req.flash('error_msg', 'Error: User not found');
                        return res.redirect('/users/showminutes');    
                    }    
                })
                .catch(err =>
                {
                    console.log(err);
                    req.flash('error_msg', 'Failed trying to find current User');
                    return res.redirect('/users/showminutes');
                });
            }
            else
            {
                req.flash('error_msg', 'Error: File not found');
                return res.redirect('/users/showminutes');
            }
        });
    }
    else
    {
        gfs.collection('uploads');
        gfs.files.findOne({ filename: docname }, (err, fileInfo) =>
        {
            if(err) throw err;

            if(fileInfo)
            {
                User.findOne({ email: currentUserEmail })
                .then(user =>
                {
                    if(user)
                    {
                        // Match password
                        bcrypt.compare(password, user.password, (err, isMatch) =>
                        {
                            if(err) throw err;

                            if(isMatch)
                            {
                                let readstream = gfs.createReadStream
                                ({
                                    filename: fileInfo.filename,
                                    root: "uploads",
                                    mode: "r"
                                });
                                
                                res.set('Content-Type', fileInfo.contentType);

                                let response = readstream.pipe(res);

                                res.on('finish', () => {return response});
                            }
                            else
                            {
                                req.flash('error_msg', 'Error: User password incorrect');
                                res.redirect('/users/showminutes');
                            }
                        });
                    }
                    else
                    {
                        req.flash('error_msg', 'Error: User not found');
                        return res.redirect('/users/showminutes');    
                    }    
                })
                .catch(err =>
                {
                    console.log(err);
                    req.flash('error_msg', 'Failed trying to find current User');
                    return res.redirect('/users/showminutes');
                });
            }
            else
            {
                req.flash('error_msg', 'Error: File not found');
                return res.redirect('/users/showminutes');
            }
        });
    }
});

// @route POST /users/decipher
// @desc Serves the deciphered file to the user
app.post('/users/decipher', (req, res) => 
{
    const {docname, key} = req.body;

    const extension = path.extname(docname);
    if(extension.localeCompare('.aes') === 0)
    {
        // gfs.files.findOne({ filename: docname }, (err, fileInfo) => 
        gfs.collection('uploadsCiphered');
        gfs.files.findOne({ filename: docname })
        .then(fileInfo => 
        {            
            // Check if file
            if(!fileInfo) 
            {
                req.flash('error_msg', 'Error: File not found');
                res.redirect('/users/showminutes');
            }

            let readstream = gfs.createReadStream
            ({
                filename: fileInfo.filename,
                root: "uploadsCiphered",
                mode: "r"
            });

            let finalFileName = path.parse(fileInfo.filename).name;
            console.log('The final file name: ' + finalFileName);

            // const key = crypto.scryptSync(password, 'salt', 24);
            // const iv = crypto.randomBytes(16);
            const decipher = crypto.createDecipher('aes-192-cbc', key);
            // let deciphered = decipher.update(readstream, 'binary', 'utf8');
            // deciphered += decipher.final('utf8');
            // fs.writeFile('random.txt', deciphered, function(err)
            // {
            //     if(err) throw err;
            //     console.log('Saved');
            // });

            // fs.readFile('random.txt', function(err, data)
            // {
            //     res.writeHead(200, {'Content-Type': 'text/plain'});
            //     res.write(data);
            //     res.end();
            // });

            res.set('Content-Type', 'application/vnd.openxmlformats-officedocument.wordprocessingml.document');

            let deciphered = readstream.pipe(decipher);

            decipher.on('error', err => 
            {
                console.log(err);
                req.flash('error_msg', 'The keys do not match, try again');
                res.redirect('/users/showminutes');
            });

            let response;
            decipher.on('finish', () => response = deciphered.pipe(res));
            
            return response;
        })
        .catch(err =>
        {
            console.log(err);
            req.flash('error_msg', 'Error: Some problem trying to find the file');
            res.redirect('/users/showminutes');
        });        
    }
    else
    {
        req.flash('error_msg', 'Error: File is not ciphered');
        return res.redirect('/users/showminutes');
    }
});

// @route POST /users/filedelete/
// @desc Renders Delete Minute
app.post('/users/filedelete/', (req, res) => 
{
    const {docname, password} = req.body;

    User.findOne({ email: currentUserEmail })
    .then(user =>
    {
        if(!user)
            res.render('showminutes.ejs', { error_msg: 'Error: User not found' });
        else
        {
            // Match password
            bcrypt.compare(password, user.password, (err, isMatch) =>
            {
                if(err) throw err;

                if(isMatch)
                {
                    if(path.extname(docname).localeCompare('.aes') === 0)
                        collection = gfs.collection('uploadsCiphered');
                    else
                        collection = gfs.collection('uploads');

                    gfs.files.deleteOne({ filename: docname }, (err, gridStore) => 
                    {
                        if (err) 
                            return res.status(404).json({ err: err });

                        req.flash('success_msg', 'File deleted successfully!');
                        res.redirect('/users/showminutes');
                    });
                }
                else
                {
                    req.flash('error_msg', 'The passwords do not match, try again');
                    res.redirect('/users/showminutes');
                }
            });
        }
    })
    .catch(err =>
    {
        console.log(err);
        req.flash('error_msg', 'Error: Problem trying to find the user');
        res.redirect('/users/showminutes');
    });        
});

// @route GET /users/confidentmemo
// @desc Renders Confident Memo EJS
app.get('/users/confidentmemo', (req, res) => 
{
    User.find({  })
    .then(users =>
    {
        res.render('confidentmemo.ejs', { users: users });
    })
    .catch(err =>
    {
        console.log(err);
        req.flash('error_msg', 'Error: Problem trying to find the User');
        res.redirect('/users/confidentmemo');
    });        
});

// @route POST /users/confidentmemo
// @desc Renders Confident Memo EJS
app.post('/users/confidentmemo', (req, res) =>
{
    let { usernamereceiver, memorandum, password, subject } = req.body;

    usernamereceiver = usernamereceiver.trim();
    password = password.trim();
    // IMPORTANT: Use findOne() when trying to access a single user, because when using find()
    // the program returns an array
    User.findOne({ username: usernamereceiver })
    .then(userReceiver =>
    {
        if(userReceiver)
        {
            User.findOne({ email: currentUserEmail })
            .then(userSender =>
            {
                if(userSender)
                {
                    // const key = crypto.scryptSync(password, 'salt', 24);
                    const cipher = crypto.createCipher('aes-192-cbc', password);

                    let encrypted = cipher.update(memorandum, 'utf8', 'binary') + cipher.final('binary');
                    console.log('Data encrypted: ' + encrypted);
                    
                    let key = new nodersa(userReceiver.publickey);
                    let passwordEncrypted = key.encrypt(password, 'base64');
                    console.log('Type of passwordEncrypted: ' + typeof passwordEncrypted);
                    console.log('Password encrypted: ' + passwordEncrypted);

                    // PRUEBA DESCIFRAR AES EN LA MISMA FUNCIÓN
                    // const decipher = crypto.createDecipher('aes-192-cbc', password);
                    // let decrypted = decipher.update(encrypted, 'binary', 'binary') + decipher.final('binary');
                    // console.log('Data decrypted: ' + decrypted);
                    // -------------------------------------------------------------

                    // LAST VERSION - WORKING
                    // let passwordBuf = Buffer.from(password, 'utf8');
                    // let passwordEncrypted = crypto.publicEncrypt({ key: userReceiver.publickey, padding: crypto.constants.RSA_PCKS1_PADDING }, passwordBuf);
                    // ---------------------------------

                    // PRUEBA DESCIFRAR RSA EN LA MISMA FUNCIÓN
                    // let passwordDecrypted = crypto.publicDecrypt({ key: privatekey, padding: crypto.constants.RSA_NO_PADDING }, passwordEncrypted);
                    // console.log('Password decrypted: ' + passwordDecrypted.toString('utf8'));
                    // -------------------------------------------------------------

                    let writestream = gfs.createWriteStream
                    ({
                        filename: subject + '.txt',
                        root: "uploadsMemosConfident",
                        mode: "w",
                        metadata: { receiver: userReceiver.username, sender: userSender.username, passwordEncrypted: passwordEncrypted, type: 'confident' }
                    });

                    writestream.write(encrypted, 'binary');
                    writestream.end();

                    req.flash('success_msg', 'Memo Sent');
                    res.redirect('/users/confidentmemo');                    
                }
                else
                {
                    req.flash('error_msg', 'User Sender not found');
                    res.redirect('/users/confidentmemo');
                }
            })
            .catch(err =>
            {
                console.log(err);
                req.flash('error_msg', 'Error: Problem trying to find the User Sender');
                res.redirect('/users/confidentmemo');
            });        
        }   
        else
        {
            req.flash('error_msg', 'User Receiver not found');
            res.redirect('/users/confidentmemo');
        } 
    })
    .catch(err =>
    {
        console.log(err);
        req.flash('error_msg', 'Error: Problem trying to find the User Receiver');
        res.redirect('/users/confidentmemo');
    });        
});

// @route GET /users/home
// @desc Renders Home EJS
app.get('/users/showmemos', (req, res) =>
{
    let memorandums = [];
    let i;

    User.findOne({ email: currentUserEmail })
    .then(currentUser =>
    {
        if(currentUser)
        {
            gfs.collection('uploadsMemosConfident');
            gfs.files.find().toArray((err, memos) => 
            {
                if(err)
                {
                    console.log(err);
                    req.flash('error_msg', 'Error: Problem ocurred trying to find Confidential Memos');
                    res.redirect('/users/showmemos');
                } 
                else
                {
                    for(i = 0; i < memos.length; i++)
                    {
                        if(memos[i].metadata.receiver === currentUser.username)
                            memorandums.push(memos[i]);
                    }

                    gfs.collection('uploadsMemos');
                    gfs.files.find().toArray((err, memos) => 
                    {
                        if(err)
                        {
                            console.log(err);
                            req.flash('error_msg', 'Error: Problem ocurred trying to find Memos');
                            res.redirect('/users/showmemos');
                        } 
                        else
                        {
                            for(i = 0; i < memos.length; i++)
                            {
                                if(memos[i].metadata.receiver === currentUser.username)
                                    memorandums.push(memos[i]);                            
                            }
                            res.render('showmemos.ejs', { memos: memorandums });
                        }
                    });
                }
            });
        }
        else
        {
            req.flash('error_msg', 'Error: Current User not found');
            res.redirect('/users/showmemos');    
        }    
    })
    .catch(err =>
    {
        console.log(err);
        req.flash('error_msg', 'Error: Problem trying to find the Current User');
        res.redirect('/users/showmemos');
    });        
});

// @route POST /users/decipher
// @desc Serves the deciphered memo to the user
app.post('/users/deciphermemo', uploadpk.single('keyfile'), (req, res) => 
{
    let {docname} = req.body;

    // gfs.files.findOne({ filename: docname }, (err, fileInfo) => 
    gfs.collection('uploadsMemosConfident');
    gfs.files.findOne({ filename: docname })
    .then(fileInfo => 
    {            
        // Check if file
        if(!fileInfo) 
        {
            req.flash('error_msg', 'Error: File not found');
            res.redirect('/users/showmemos');
        }
        else
        {
            let readstream = gfs.createReadStream
            ({
                filename: fileInfo.filename,
                root: "uploadsMemosConfident",
                mode: "r"
            });

            toArray(readstream)
            .then(function (parts) 
            {
                const buffers = parts
                .map(part => util.isBuffer(part) ? part : Buffer.from(part));
                let buffer = Buffer.concat(buffers);

                const key = new nodersa(req.file.buffer);
                let password = key.decrypt(fileInfo.metadata.passwordEncrypted, 'utf8');
                console.log('Password decrypted: ' + password);

                // LAST VERSION
                const decipher = crypto.createDecipher('aes-192-cbc', password);

                let decrypted = decipher.update(buffer, 'binary', 'binary') + decipher.final();
                console.log('Data decrypted: ' + decrypted);

                fs.writeFile('deciphered.txt', decrypted, (err) =>
                {
                    if(err) throw err;
                    console.log('Saved to user file system!');
                });

                res.set('Content-Type', 'plain/text');
                let readstream = fs.createReadStream('./deciphered.txt');

                return readstream.pipe(res);
                // let deciphered = readstream.pipe(decipher);

                // decipher.on('error', err => 
                // {
                //     console.log(err);
                //     req.flash('error_msg', 'The keys do not match, try again');
                //     res.redirect('/users/showmemos');
                // });

                // let response;
                // decipher.on('finish', () => response = deciphered.pipe(res));
                
                //return response;
            })
            .catch(err =>
            {
                console.log(err);
                req.flash('error_msg', 'Error: Key is incorrect');
                res.redirect('/users/showmemos');
            });        
        }
    })
    .catch(err =>
    {
        console.log(err);
        req.flash('error_msg', 'Error: Some problem trying to find the Confident Memo');
        res.redirect('/users/showmemos');
    });        
});

// @route POST /users/filedeletememo/
// @desc Renders Delete Memo
app.post('/users/filedeletememo', (req, res) => 
{
    const {docname, type, password} = req.body;

    if(type === 'confident')
    {
        User.findOne({ email: currentUserEmail })
        .then(user =>
        {
            if(!user)
                res.render('showmemos.ejs', { error_msg: 'Error: User not found' });
            else
            {
                // Match password
                bcrypt.compare(password, user.password, (err, isMatch) =>
                {
                    if(err) throw err;

                    if(isMatch)
                    {
                        collection = gfs.collection('uploadsMemosConfident');
                        gfs.files.deleteOne({ filename: docname }, (err, gridStore) => 
                        {
                            if (err) 
                                return res.status(404).json({ err: err });

                            req.flash('success_msg', 'Confident Memo deleted successfully!');
                            res.redirect('/users/showmemos');
                            // res.render('showmemos.ejs', { success_msg: 'Memo deleted successfully!', memos });
                        });
                    }
                    else
                    {
                        req.flash('error_msg', 'The passwords do not match, try again');
                        res.redirect('/users/showmemos');
                        // res.render('showmemos.ejs', { error_msg: 'Passwords do not match, try again' });
                    }
                });
            }
        })
        .catch(err =>
        {
            console.log(err);
            req.flash('error_msg', 'Error: Problem trying to find the user');
            res.redirect('/users/showmemos');
        });        
    }
    else
    {
        User.findOne({ email: currentUserEmail })
        .then(user =>
        {
            if(!user)
                res.render('showmemos.ejs', { error_msg: 'Error: User not found' });
            else
            {
                // Match password
                bcrypt.compare(password, user.password, (err, isMatch) =>
                {
                    if(err) throw err;

                    if(isMatch)
                    {
                        collection = gfs.collection('uploadsMemos');
                        gfs.files.deleteOne({ filename: docname }, (err, gridStore) => 
                        {
                            if (err) 
                                return res.status(404).json({ err: err });

                            req.flash('success_msg', 'Memo deleted successfully!');
                            res.redirect('/users/showmemos');
                            // res.render('showmemos.ejs', { success_msg: 'Memo deleted successfully!', memos });
                        });
                    }
                    else
                    {
                        req.flash('error_msg', 'The passwords do not match, try again');
                        res.redirect('/users/showmemos');
                        // res.render('showmemos.ejs', { error_msg: 'Passwords do not match, try again' });
                    }
                });
            }
        })
        .catch(err =>
        {
            console.log(err);
            req.flash('error_msg', 'Error: Problem trying to find the user');
            res.redirect('/users/showmemos');
        });        
    }
});

// @route POST /users/downloadmemo
// @desc Serves the Memo to be downloaded
app.post('/users/downloadmemo', (req, res) =>
{
    const { docname, type, password } = req.body;

    if(type === 'confident')
    {
        gfs.collection('uploadsMemosConfident');
        gfs.files.findOne({ filename: docname }, (err, fileInfo) =>
        {
            if(err) throw err;

            if(fileInfo)
            {
                User.findOne({ email: currentUserEmail })
                .then(user =>
                {
                    if(user)
                    {
                        // Match password
                        bcrypt.compare(password, user.password, (err, isMatch) =>
                        {
                            if(err) throw err;

                            if(isMatch)
                            {
                                let readstream = gfs.createReadStream
                                ({
                                    filename: fileInfo.filename,
                                    root: "uploadsMemoConfident",
                                    mode: "r"
                                });
                                
                                res.set('Content-Type', 'plain/text');

                                let response = readstream.pipe(res);

                                res.on('finish', () => {return response});
                            }
                            else
                            {
                                req.flash('error_msg', 'Error: User password incorrect');
                                res.redirect('/users/showmemos');
                            }
                        });
                    }
                    else
                    {
                        req.flash('error_msg', 'Error: User not found');
                        return res.redirect('/users/showmemos');    
                    }    
                })
                .catch(err =>
                {
                    console.log(err);
                    req.flash('error_msg', 'Failed trying to find current User');
                    return res.redirect('/users/showmemos');
                });
            }
            else
            {
                req.flash('error_msg', 'Error: File not found');
                return res.redirect('/users/showmemos');
            }
        });
    }
    else
    {
        gfs.collection('uploadsMemos');
        gfs.files.findOne({ filename: docname }, (err, fileInfo) =>
        {
            if(err) throw err;

            if(fileInfo)
            {
                User.findOne({ email: currentUserEmail })
                .then(user =>
                {
                    if(user)
                    {
                        // Match password
                        bcrypt.compare(password, user.password, (err, isMatch) =>
                        {
                            if(err) throw err;

                            if(isMatch)
                            {
                                let readstream = gfs.createReadStream
                                ({
                                    filename: fileInfo.filename,
                                    root: "uploadsMemos",
                                    mode: "r"
                                });
                                
                                res.set('Content-Type', 'plain/text');

                                let response = readstream.pipe(res);

                                res.on('finish', () => {return response});
                            }
                            else
                            {
                                req.flash('error_msg', 'Error: User password incorrect');
                                res.redirect('/users/showmemos');
                            }
                        });
                    }
                    else
                    {
                        req.flash('error_msg', 'Error: User not found');
                        return res.redirect('/users/showmemos');    
                    }    
                })
                .catch(err =>
                {
                    console.log(err);
                    req.flash('error_msg', 'Failed trying to find current User');
                    return res.redirect('/users/showmemos');
                });
            }
            else
            {
                req.flash('error_msg', 'Error: File not found');
                return res.redirect('/users/showmemos');
            }
        });
    }
});

// @route GET /users/memo
// @desc Renders Memo EJS
app.get('/users/memo', (req, res) => 
{
    User.find({  })
    .then(users =>
    {
        res.render('memo.ejs', { users: users });
    })
    .catch(err =>
    {
        console.log(err);
        req.flash('error_msg', 'Error: Problem trying to find the User');
        res.redirect('/users/memo');
    });        
});

// @route GET /users/memo
// @desc Renders Memo EJS
app.post('/users/memo', uploadpk.single('keyfile'), (req, res) =>
{
    let { usernamereceiver, subject, memorandum } = req.body;
    
    usernamereceiver = usernamereceiver.trim();
    User.findOne({ email: currentUserEmail })
    .then(userSender =>
    {
        if(userSender)
        {
            User.findOne({ username: usernamereceiver })
            .then(userReceiver =>
            {
                const bufferMemo = Buffer.from(memorandum, 'utf8');

                // Paso 1 Emisor
                const sign = crypto.createSign('SHA256');
                sign.write(bufferMemo);
                sign.end();

                // Paso 2 Emisor
                const signature = sign.sign(req.file.buffer);

                // Paso 1 Receptor
                const verify = crypto.createVerify('SHA256');
                verify.write(bufferMemo);
                verify.end();

                // Paso 2 Receptor
                if(verify.verify(userSender.publickey, signature))
                {
                    let writestream = gfs.createWriteStream
                    ({
                        filename: subject + '.txt',
                        root: "uploadsMemos",
                        mode: "w",
                        metadata: { receiver: userReceiver.username, sender: userSender.username, type: 'common' }
                    });

                    writestream.write(memorandum, 'utf8');
                    writestream.end();

                    req.flash('success_msg', 'You have signed up correctly!. And the system has verified your signature. Memo sent');
                    res.redirect('/users/memo');                    
                }
                else
                {
                    req.flash('error_msg', 'The keys do not match, try again');
                    res.redirect('/users/memo');
                }
            })
            .catch(err =>
            {
                console.log(err);
                req.flash('error_msg', 'Error: Problem trying to find the User receiver');
                res.redirect('/users/memo');
            });        
        }
        else
        {
            req.flash('error_msg', 'Error: Current user not found');
            res.redirect('/users/memo');
        }
    })
    .catch(err =>
    {
        console.log(err);
        req.flash('error_msg', 'Error: Problem trying to find the Current User');
        res.redirect('/users/memo');
    });        
});

// @route GET /users/home
// @desc Renders Home EJS
app.get('/users/home', (req, res) => res.render('home.ejs', { typeUser: currentUserType }));

// Templates and API Routes
//app.use('/users', require('./routes/users'));
app.use('/users', usersModule.router);

// API Routes
// app.use('/api/users', require('./routes/api/users'));

const PORT = process.env.PORT || 5000;

app.listen(PORT, () => console.log(`Server started on port ${PORT}`));
